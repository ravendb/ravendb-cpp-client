#include "stdafx.h"
#include "DocumentStoreBase.h"
#include "DocumentConventions.h"
#include "AbstractIndexCreationTaskBase.h"
#include "IndexCreation.h"
#include "MaintenanceOperationExecutor.h"
#include "PutIndexesOperation.h"
#include "EventHelper.h"
#include "SessionCreatedEventArgs.h"

namespace ravendb::client::documents
{
	DocumentStoreBase::~DocumentStoreBase() = default;

	bool DocumentStoreBase::is_disposed() const
	{
		return disposed;
	}

	void DocumentStoreBase::execute_index(std::shared_ptr<indexes::AbstractIndexCreationTaskBase> task,
		std::optional<std::string> database)
	{
		assert_initialized();
		task->execute(_weak_this.lock(), _conventions, std::move(database));
	}

	void DocumentStoreBase::execute_indexes(
		std::vector<std::shared_ptr<indexes::AbstractIndexCreationTaskBase>>& tasks,
		std::optional<std::string> database)
	{
		assert_initialized();
		auto&& indexes_to_add = indexes::IndexCreation::create_indexes_to_add(tasks, _conventions);

		maintenance()->for_database(database ? *std::move(database) : get_database())
			->send(operations::indexes::PutIndexesOperation(std::move(indexes_to_add)));
	}

	std::shared_ptr<conventions::DocumentConventions> DocumentStoreBase::get_conventions() const
	{
		return _conventions ? _conventions : _conventions = conventions::DocumentConventions::create();
	}

	void DocumentStoreBase::set_conventions(std::shared_ptr<conventions::DocumentConventions> conventions)
	{
		assert_not_initialized("conventions");
		_conventions = conventions;
	}

	const std::vector<std::string>& DocumentStoreBase::get_urls() const
	{
		return urls;
	}

	void DocumentStoreBase::set_urls(std::vector<std::string> urls_param)
	{
		assert_not_initialized("urls");

		if(urls_param.empty())
		{
			throw std::invalid_argument("urls can't be empty");
		}
		for(auto& url : urls_param)
		{
			if (url.empty())
			{
				throw std::invalid_argument("url can't be empty");
			}

			if (url.back() == '/')
			{
				url.pop_back();
			}
		}
		urls = std::move(urls_param);
	}

	std::optional<int64_t> DocumentStoreBase::get_last_transaction_index(const std::string& database) const
	{
		auto lock = std::lock_guard(_transaction_index_mutex);
		if (auto it = _last_raft_index_per_database.find(database);
			it != _last_raft_index_per_database.end())
		{
			auto&& index = it->second;
			if (!index || *index == 0)
			{
				return {};
			}
			return index;
		}
		return {};
	}

	void DocumentStoreBase::set_last_transaction_index(const std::string& database, std::optional<int64_t> index)
	{
		if (!index)
		{
			return;
		}

		auto lock = std::lock_guard(_transaction_index_mutex);

		if (auto it = _last_raft_index_per_database.find(database);
			it != _last_raft_index_per_database.end() && it->second.has_value())
		{
			//undef Windows macro
#ifdef max
#undef max
#endif
			it->second = std::max(it->second.value(), index.value());
		}
		else
		{
			_last_raft_index_per_database.insert_or_assign(database, index);
		}
	}

	void DocumentStoreBase::ensure_not_closed() const
	{
		if(disposed)
		{
			throw std::runtime_error("The document store has already been closed and cannot be used");
		}
	}

	void DocumentStoreBase::after_session_created(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) const
	{
		primitives::EventHelper::invoke(on_session_created, *this, primitives::SessionCreatedEventArgs(session));
	}

	void DocumentStoreBase::register_events(session::InMemoryDocumentSessionOperations& session) const
	{
		for(const auto& handler : on_before_store)
		{
			session.add_before_store_listener(handler);
		}
		for (const auto& handler : on_after_save_changes)
		{
			session.add_after_save_changes_listener(handler);
		}
		for (const auto& handler : on_before_delete)
		{
			session.add_before_delete_listener(handler);
		}
		for (const auto& handler : on_before_query)
		{
			session.add_before_query_listener(handler);
		}
	}

	void DocumentStoreBase::add_before_store_listener(primitives::EventHandler handler)
	{
		on_before_store.emplace_back(std::move(handler));
	}

	void DocumentStoreBase::assert_initialized() const
	{
		if(!is_initialized)
		{
			throw std::runtime_error("You cannot open a session or access the database commands before initializing the document store."
				" Did you forget calling initialize()?");
		}
	}

	void DocumentStoreBase::assert_not_initialized(const std::string& property) const
	{
		if(is_initialized)
		{
			std::ostringstream message{};
			message << "You cannot set '" << property << "' after the document store has been initialized.";
			throw std::runtime_error(message.str());
		}
	}

	void DocumentStoreBase::remove_before_store_listener(const primitives::EventHandler& handler)
	{
		if (const auto it = std::find(on_before_store.cbegin(), on_before_store.cend(), handler);
			it != on_before_store.cend())
		{
			on_before_store.erase(it);
		}
	}

	void DocumentStoreBase::add_after_save_changes_listener(primitives::EventHandler handler)
	{
		on_after_save_changes.emplace_back(std::move(handler));
	}

	void DocumentStoreBase::remove_after_save_changes_listener(const primitives::EventHandler& handler)
	{
		if (const auto it = std::find(on_after_save_changes.cbegin(), on_after_save_changes.cend(), handler);
			it != on_after_save_changes.cend())
		{
			on_after_save_changes.erase(it);
		}
	}

	void DocumentStoreBase::add_before_delete_listener(primitives::EventHandler handler)
	{
		on_before_delete.emplace_back(std::move(handler));
	}

	void DocumentStoreBase::remove_before_delete_listener(const primitives::EventHandler& handler)
	{
		if (const auto it = std::find(on_before_delete.cbegin(), on_before_delete.cend(), handler);
			it != on_before_delete.cend())
		{
			on_before_delete.erase(it);
		}
	}

	void DocumentStoreBase::add_before_query_listener(primitives::EventHandler handler)
	{
		on_before_query.emplace_back(std::move(handler));
	}

	void DocumentStoreBase::remove_before_query_listener(const primitives::EventHandler& handler)
	{
		if (const auto it = std::find(on_before_query.cbegin(), on_before_query.cend(), handler);
			it != on_before_query.cend())
		{
			on_before_query.erase(it);
		}
	}

	const std::string& DocumentStoreBase::get_database() const
	{
		return database;
	}

	void DocumentStoreBase::set_database(std::string database_param)
	{
		assert_not_initialized("database");
		database = std::move(database_param);
	}

	const std::optional<impl::CertificateDetails>& DocumentStoreBase::get_certificate_details() const
	{
		return _certificate_details;
	}

	void DocumentStoreBase::set_certificate_details(impl::CertificateDetails certificate_details)
	{
		_certificate_details = std::move(certificate_details);
	}

	const impl::CurlOptionsSetter& DocumentStoreBase::get_before_perform() const
	{
		return _set_before_perform;
	}

	void DocumentStoreBase::set_before_perform(impl::CurlOptionsSetter options_setter)
	{
		_set_before_perform = std::move(options_setter);
	}

	const impl::CurlOptionsSetter& DocumentStoreBase::get_after_perform() const
	{
		return _set_after_perform;
	}

	void DocumentStoreBase::set_after_perform(impl::CurlOptionsSetter options_setter)
	{
		_set_after_perform = std::move(options_setter);
	}
}
