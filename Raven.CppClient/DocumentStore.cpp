#include "stdafx.h"
#include "DocumentStore.h"
#include "DocumentSession.h"
#include "MaintenanceOperationExecutor.h"

namespace ravendb::client::documents
{
	DocumentStore::~DocumentStore() = default;

	DocumentStore::DocumentStore() = default;

	DocumentStore::DocumentStore(std::string url, std::string database)
		: DocumentStore(std::vector<std::string>{std::move(url)}, std::move(database))
	{}

	DocumentStore::DocumentStore(std::vector<std::string> urls, std::string database)
	{
		set_urls(std::move(urls));
		set_database(std::move(database));
	}

	std::string DocumentStore::get_identifier() const
	{
		if(!_identifier.empty())
		{
			return _identifier;
		}

		std::ostringstream result{};
		std::copy(urls.cbegin(), urls.cend(), std::ostream_iterator<std::string>(result, ","));		
		if(!database.empty())
		{
			result << " (DB: " << database << ")";
		}

		return result.str();
	}

	void DocumentStore::set_identifier(std::string identifier)
	{
		_identifier = std::move(identifier);
	}

	session::DocumentSession DocumentStore::open_session()
	{
		return this->open_session(session::SessionOptions());
	}
	class session::DocumentSession DocumentStore::open_session(const std::string& database)
	{
		return open_session(session::SessionOptions{ database });
	}
	session::DocumentSession DocumentStore::open_session(const session::SessionOptions& options)
	{
		assert_initialized();
		//TODO ensure_not_closed();

		session::DocumentSession session(*this, options);
		//TODO
		//register_events(session);
		//after_session_created(session);
		return session;
	}

	std::shared_ptr<http::RequestExecutor> DocumentStore::get_request_executor(const std::string& database) const
	{
		assert_initialized();

		auto&& db_name = database.empty() ? get_database() : database;

		{
			auto lock = std::shared_lock(_request_executors_mutex);
			if(auto it = _request_executors.find(database);
				it != _request_executors.end())
			{
				return it->second;
			}
		}
		{
			auto lock = std::unique_lock(_request_executors_mutex);
			std::shared_ptr<http::RequestExecutor> re = http::RequestExecutor::create(
				get_urls(), db_name, _certificate_details, _set_before_perform, _set_after_perform);
			_request_executors.emplace(database, re);
			return re;
		}
	}

	void DocumentStore::assert_valid_configuration() const
	{
		if(urls.empty())
		{
			throw std::invalid_argument("Document store URLs cannot be empty");
		}
	}

	IDocumentStore& DocumentStore::initialize()
	{
		if(is_initialized)
		{
			return *this;
		}
		assert_valid_configuration();

		//TODO take care of the HiLo
		is_initialized = true;
		return *this;
	}

	std::shared_ptr<operations::MaintenanceOperationExecutor> DocumentStore::get_maintenance() const
	{
		return _maintenance_operation_executor;
	}

	std::shared_ptr<operations::OperationExecutor> DocumentStore::get_operations() const
	{
		return _operation_executor;
	}
}
