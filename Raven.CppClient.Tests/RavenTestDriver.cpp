#include "pch.h"
#include "RavenTestDriver.h"
#include "DocumentConventions.h"
#include "raven_test_definitions.h"
#include "CreateDatabaseOperation.h"
#include "RequestExecutor.h"
#include "DeleteDatabasesOperation.h"
#include "SimpleStopWatch.h"
#include "GetStatisticsOperation.h"
#include "MaintenanceOperationExecutor.h"

namespace ravendb::client::tests::driver
{
	const infrastructure::ConnectionDetailsHolder RavenTestDriver::secured_connection_details = 
		infrastructure::ConnectionDetailsHolder(
			infrastructure::SECURED_RE_DETAILS_FILE_NAME, true);

	const infrastructure::ConnectionDetailsHolder RavenTestDriver::unsecured_connection_details = 
		infrastructure::ConnectionDetailsHolder(
			infrastructure::UNSECURED_RE_DETAILS_FILE_NAME, false);

	std::shared_ptr<documents::IDocumentStore> RavenTestDriver::global_server{};

	std::shared_ptr<documents::IDocumentStore> RavenTestDriver::global_secured_server{};

	std::mutex RavenTestDriver::documents_stores_guard{};
	std::unordered_map<std::string, std::shared_ptr<documents::DocumentStore>> RavenTestDriver::document_stores{};

	std::atomic_uint64_t RavenTestDriver::index = 1;

	std::string RavenTestDriver::generate_database_name() const
	{
		return impl::utils::GetCppClassName::get_simple_class_name(typeid(decltype(*this)));
	}

	std::shared_ptr<documents::IDocumentStore> RavenTestDriver::get_global_server(bool secured) const
	{
		return secured ? global_secured_server : global_server;
	}

	void RavenTestDriver::TearDown()
	{
		this->close();

		//TODO this is until listeners are implemented
		{
			auto guard = std::lock_guard(documents_stores_guard);

			for (auto&[identifier, store] : document_stores)
			{
				auto delete_database_operation = serverwide::operations::DeleteDatabasesOperation(store->get_database(), true);
				store->get_request_executor()->execute(delete_database_operation.get_command(store->get_conventions()));
			}
			document_stores.clear();
		}
		//TODO ---------------------------------------
	}

	RavenTestDriver::~RavenTestDriver() = default;

	bool RavenTestDriver::is_disposed() const
	{
		return disposed;
	}

	std::shared_ptr<documents::DocumentStore> RavenTestDriver::get_secured_document_store()
	{
		return get_document_store(generate_database_name(), true, {});
	}

	std::shared_ptr<documents::DocumentStore> RavenTestDriver::get_secured_document_store(const std::string& database)
	{
		return get_document_store(database, true, {});
	}

	std::shared_ptr<documents::DocumentStore> RavenTestDriver::get_document_store()
	{
		return get_document_store(generate_database_name());
	}

	std::shared_ptr<documents::DocumentStore> RavenTestDriver::get_document_store(const std::string& database)
	{
		return get_document_store(database, false, {});
	}

	std::shared_ptr<documents::DocumentStore> RavenTestDriver::get_document_store(const std::string& database,
		bool secured, const std::optional<std::chrono::milliseconds>& wait_for_indexing_timeout)
	{
		const std::string database_name = database + "_" + std::to_string(index.fetch_add(1));

		static std::mutex m{};
		if(!get_global_server(secured))
		{
			auto guard = std::lock_guard(m);
			if(!get_global_server(secured))
			{
				if (secured)
				{
					global_secured_server = documents::DocumentStore::create();
					auto server = std::static_pointer_cast<documents::DocumentStoreBase>(global_secured_server);
					server->set_urls({ secured_connection_details.get_url() });
					server->set_certificate_details(secured_connection_details.get_certificate_details());
				}
				else
				{
					global_server = documents::DocumentStore::create();
					auto server = std::static_pointer_cast<documents::DocumentStoreBase>(global_server);
					server->set_urls({ unsecured_connection_details.get_url() });
				}
				get_global_server(secured)->initialize();
			}
		}

		auto document_store = get_global_server(secured);
		auto database_record = serverwide::DatabaseRecord();
		database_record.database_name = database_name;

		customise_db_record(database_record);

		auto create_database_operation = serverwide::operations::CreateDatabaseOperation(database_record);
		//TODO implement using documentStore.maintenance().server().send(createDatabaseOperation);
		document_store->get_request_executor()->execute(
			create_database_operation.get_command(document_store->get_conventions()));

		auto store = documents::DocumentStore::create(document_store->get_urls(), database_name);
		if(secured)
		{
			store->set_certificate_details(secured_connection_details.get_certificate_details());
		}

		customise_store(store);

		//TODO hookLeakedConnectionCheck(store);

		store->initialize();

		//TODO  store.addAfterCloseListener(((sender, event)

		set_up_database(store);

		if(wait_for_indexing_timeout)
		{
			wait_for_indexing(store, database_name, wait_for_indexing_timeout);
		}

		{
			auto guard = std::lock_guard(documents_stores_guard);
			document_stores.insert_or_assign(store->get_identifier(), store);
		}

		return store;

	}

	void RavenTestDriver::wait_for_indexing(std::shared_ptr<documents::IDocumentStore> store,
		const std::optional<std::string>& database, const std::optional<std::chrono::milliseconds>& timeout)
	{
		std::chrono::milliseconds wait_timeout = timeout ? *timeout : std::chrono::minutes(1);

		impl::SimpleStopWatch sp{};

		while(sp.millis_elapsed() < wait_timeout)
		{
			auto database_statistics = store->get_request_executor(store->get_database())->
				execute(documents::operations::GetStatisticsOperation().get_command(store->get_conventions()));

			std::vector<std::reference_wrapper<documents::operations::IndexInformation>> indexes{};

			for(auto& index : database_statistics->indexes)
			{
				if(index.state != documents::indexes::IndexState::DISABLED)
				{
					indexes.emplace_back(std::ref(index));
				}
			}

			if(std::all_of(indexes.cbegin(), indexes.cend(),
				[](const std::reference_wrapper<documents::operations::IndexInformation>& index_ref)
			{
				auto prefix = std::string_view(constants::documents::indexing::SIDE_BY_SIDE_INDEX_NAME_PREFIX);
				return  !index_ref.get().is_stale &&
					index_ref.get().name.substr(0, prefix.length()) != prefix;
			}))
			{
				return;
			}

			if(std::any_of(indexes.cbegin(), indexes.cend(),
				[](const std::reference_wrapper<documents::operations::IndexInformation>& index_ref)
			{
				return index_ref.get().state == documents::indexes::IndexState::ERRONEOUS;
			}))
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	void RavenTestDriver::close()
	{
		if(disposed)
		{
			return;
		}

		std::vector<std::string_view> exceptions{};
		{
			auto guard = std::lock_guard(documents_stores_guard);
			for(auto& [identifier, store] : document_stores)
			{
				try
				{
					store->close();
				}
				catch (std::exception& e)
				{
					exceptions.emplace_back(e.what());
				}
			}
		}
		disposed = true;

		std::ostringstream msgs{};
		std::transform(exceptions.cbegin(), exceptions.cend(), 
			std::ostream_iterator<std::string_view>(msgs, ", "), [](auto& str)
		{
			return str;
		});

		if(!exceptions.empty())
		{
			throw std::runtime_error(msgs.str());
		}
	}
}
