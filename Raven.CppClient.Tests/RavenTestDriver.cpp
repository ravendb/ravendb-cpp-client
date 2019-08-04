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
#include "DatabaseDoesNotExistException.h"
#include "NoLeaderException.h"
#include "GetIndexErrorsOperation.h"
#include "TimeoutException.h"

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
	std::unordered_set<std::shared_ptr<documents::DocumentStore>> RavenTestDriver::document_stores{};

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
		try
		{
			this->close();
		}
		catch (...)
		{}
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
					customise_store(std::static_pointer_cast<documents::DocumentStore>(server));
					server->get_conventions()->set_disable_topology_updates(true);
				}
				else
				{
					global_server = documents::DocumentStore::create();
					auto server = std::static_pointer_cast<documents::DocumentStoreBase>(global_server);
					server->set_urls({ unsecured_connection_details.get_url() });
					customise_store(std::static_pointer_cast<documents::DocumentStore>(server));
					server->get_conventions()->set_disable_topology_updates(true);
				}
				get_global_server(secured)->initialize();
			}
		}

		auto document_store = get_global_server(secured);
		auto database_record = serverwide::DatabaseRecord();
		database_record.database_name = database_name;

		customise_db_record(database_record);

		document_store->maintenance()->server()->send(serverwide::operations::CreateDatabaseOperation(database_record));

		auto store = documents::DocumentStore::create(document_store->get_urls(), database_name);
		if(secured)
		{
			store->set_certificate_details(secured_connection_details.get_certificate_details());
		}

		customise_store(store);

		//TODO hookLeakedConnectionCheck(store);

		store->initialize();

		store->add_after_close_listener(std::function<void(const documents::DocumentStore*, const primitives::VoidArgs&)>(
			[store](const documents::DocumentStore*, const primitives::EventArgs&)
		{
			if(document_stores.find(store) == document_stores.end())
			{
				return;
			}
			try
			{
				store->maintenance()->server()->send(
					serverwide::operations::DeleteDatabasesOperation(store->get_database(), true));
			}
			catch (exceptions::database::DatabaseDoesNotExistException&)
			{}
			catch (exceptions::cluster::NoLeaderException&)
			{}
		}));

		set_up_database(store);

		if(wait_for_indexing_timeout)
		{
			wait_for_indexing(store, database_name, wait_for_indexing_timeout);
		}

		{
			auto guard = std::lock_guard(documents_stores_guard);
			document_stores.insert(store);
		}

		return store;
	}

	void RavenTestDriver::wait_for_indexing(std::shared_ptr<documents::IDocumentStore> store,
		const std::optional<std::string>& database, const std::optional<std::chrono::milliseconds>& timeout)
	{
		auto admin = store->maintenance()->for_database(database.value_or(""));

		std::chrono::milliseconds wait_timeout = timeout ? *timeout : std::chrono::minutes(1);

		impl::SimpleStopWatch sp{};

		while (sp.millis_elapsed() < wait_timeout)
		{
			auto op = documents::operations::GetStatisticsOperation();

			auto database_statistics = admin->send(op);

			std::vector<std::reference_wrapper<documents::operations::IndexInformation>> indexes{};

			for (auto& index : database_statistics->indexes)
			{
				if (index.state != documents::indexes::IndexState::DISABLED)
				{
					indexes.emplace_back(std::ref(index));
				}
			}

			if (std::all_of(indexes.cbegin(), indexes.cend(),
				[](const std::reference_wrapper<documents::operations::IndexInformation>& index_ref)
			{
				auto prefix = std::string_view(constants::documents::indexing::SIDE_BY_SIDE_INDEX_NAME_PREFIX);
				return  !index_ref.get().is_stale &&
					index_ref.get().name.substr(0, prefix.length()) != prefix;
			}))
			{
				return;
			}

			if (std::any_of(indexes.cbegin(), indexes.cend(),
				[](const std::reference_wrapper<documents::operations::IndexInformation>& index_ref)
			{
				return index_ref.get().state == documents::indexes::IndexState::ERRONEOUS;
			}))
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		auto errors = admin->send(documents::operations::indexes::GetIndexErrorsOperation());

		auto format_index_errors = [](const documents::indexes::IndexErrors& index_errors)->std::string
		{
			std::ostringstream errors_list_text{};
			for(const auto& error : index_errors.errors)
			{
				errors_list_text << "-" << error.to_string() << "\r\n";
			}
			std::ostringstream res{};
			res << "Index " << index_errors.name << " (" << 
				index_errors.errors.size() << " errors): " << "\r\n" << errors_list_text.str();
			return res.str();
		};

		std::ostringstream all_index_errors_text{};
		if(errors && !errors->empty())
		{
			for(const auto& error : *errors)
			{
				all_index_errors_text << format_index_errors(error) << "\r\n";
			}
		}

		std::ostringstream msg{};
		msg << "The indexes stayed stale for more than " << impl::utils::MillisToTimeSpanConverter(wait_timeout) << "." <<
			all_index_errors_text.str();

		throw exceptions::TimeoutException(msg.str());
	}

	void RavenTestDriver::close()
	{
		if(disposed)
		{
			return;
		}

		std::vector<std::string> exceptions{};
		{
			auto guard = std::lock_guard(documents_stores_guard);
			for(auto& store : document_stores)
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
		std::copy(exceptions.cbegin(), exceptions.cend(), 
			std::ostream_iterator<std::string>(msgs, ", "));

		if(!exceptions.empty())
		{
			throw std::runtime_error(msgs.str());
		}
	}
}
