#include "stdafx.h"
#include "DocumentStore.h"
#include "DocumentSession.h"
#include "SessionOptions.h"
#include "MaintenanceOperationExecutor.h"

namespace ravendb::client::documents
{
	DocumentStore::~DocumentStore() = default;

	std::shared_ptr<DocumentStore> DocumentStore::create()
	{
		auto object = std::shared_ptr<DocumentStore>(new DocumentStore());
		object->_weak_this = object;
		object->initiate_operation_executors();
		return object;
	}

	std::shared_ptr<DocumentStore> DocumentStore::create(std::string url, std::string database)
	{
		auto object = std::shared_ptr<DocumentStore>(new DocumentStore(std::move(url), std::move(database)));
		object->_weak_this = object;
		object->initiate_operation_executors();
		return object;
	}

	std::shared_ptr<DocumentStore> DocumentStore::create(std::vector<std::string> urls, std::string database)
	{
		auto object = std::shared_ptr<DocumentStore>(new DocumentStore(std::move(urls), std::move(database)));
		object->_weak_this = object;
		object->initiate_operation_executors();
		return object;
	}

	void DocumentStore::initiate_operation_executors()
	{
		_maintenance_operation_executor = std::make_shared<operations::MaintenanceOperationExecutor>(_weak_this.lock());
		_operation_executor = std::make_shared<operations::OperationExecutor>(_weak_this.lock());
	}

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
		return open_session(session::SessionOptions());
	}
	class session::DocumentSession DocumentStore::open_session(const std::string& database)
	{
		return open_session(session::SessionOptions{ database });
	}
	session::DocumentSession DocumentStore::open_session(const session::SessionOptions& options)
	{
		assert_initialized();
		//TODO ensure_not_closed();

		auto session_impl = std::make_shared<session::DocumentSessionImpl>(_weak_this.lock(), options);
		//TODO
		//register_events(session_impl);
		//after_session_created(session_impl);
		return session::DocumentSession(session_impl);
	}

	std::shared_ptr<http::RequestExecutor> DocumentStore::get_request_executor(const std::string& database) const
	{
		assert_initialized();

		auto&& db_name = database.empty() ? get_database() : database;

		{
			auto lock = std::shared_lock(_request_executors_mutex);
			if(auto it = _request_executors.find(db_name);
				it != _request_executors.end())
			{
				return it->second;
			}
		}
		{
			auto lock = std::unique_lock(_request_executors_mutex);
			std::shared_ptr<http::RequestExecutor> re = http::RequestExecutor::create(
				get_urls(), db_name, get_conventions(), _certificate_details, _set_before_perform, _set_after_perform);
			_request_executors.insert({ db_name, re });
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

	std::shared_ptr<IDocumentStore> DocumentStore::initialize()
	{
		if(is_initialized)
		{
			return _weak_this.lock();
		}
		assert_valid_configuration();

		//TODO take care of the HiLo
		is_initialized = true;
		return _weak_this.lock();
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
