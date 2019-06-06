#include "stdafx.h"
#include "OperationExecutor.h"
#include "DocumentStore.h"
#include "CompareStringsEqualIgnoreCase.h"
#include "VoidRavenCommand.h"

namespace ravendb::client::documents::operations
{
	OperationExecutor::OperationExecutor(std::shared_ptr<DocumentStoreBase> store, const std::optional<std::string>& database_name)
		:	OperationExecutor(std::static_pointer_cast<IDocumentStore>(store), database_name)
	{}

	OperationExecutor::OperationExecutor(std::shared_ptr<IDocumentStore> store,
		const std::optional<std::string>& database_name)
		: _store(store)
		, _database_name(database_name ? *database_name : store->get_database())
		, _request_executor(store->get_request_executor(this->_database_name))
	{}

	std::shared_ptr<OperationExecutor> OperationExecutor::create(std::shared_ptr<DocumentStoreBase> store,
		const std::optional<std::string>& database_name)
	{
		auto object = std::shared_ptr<OperationExecutor>(new OperationExecutor(store, database_name));
		object->_weak_this = object;
		return object;
	}

	std::shared_ptr<OperationExecutor> OperationExecutor::create(std::shared_ptr<IDocumentStore> store,
		const std::optional<std::string>& database_name)
	{
		auto object = std::shared_ptr<OperationExecutor>(new OperationExecutor(store, database_name));
		object->_weak_this = object;
		return object;
	}

	std::shared_ptr<OperationExecutor> OperationExecutor::for_database(const std::string& database_name) const
	{
		if(impl::utils::CompareStringsEqualIgnoreCase()(_database_name, database_name))
		{
			return _weak_this.lock();
		}

		return create(_store.lock(), database_name);
	}

	 void OperationExecutor::send(IVoidOperation& operation, const std::optional<session::SessionInfo>& session_info)
	{
		auto command = operation.get_command(_store.lock(), _request_executor->get_conventions(),_request_executor->get_cache());
		_request_executor->execute(static_cast<http::VoidRavenCommand&>(*command), session_info);
	}

	 //TODO wait for Operation implementation
	//Operation OperationExecutor::send_async(IOperation<OperationIdResult>& operation,
	//	 const std::optional<session::SessionInfo>& session_info = {})
	//{
	//	 http::HttpCache temp_cache{};
	//	 //TODO use real HttpCache
	//	 auto command = operation.get_command(_store.lock(), _request_executor->get_conventions(), temp_cache);
	//	 _request_executor->execute(*command, session_info);

	//	 return Operation(_request_executor, []() {_store.lock()->changes()}, _request_executor->get_conventions(),
	//		 command->get_result()->operation_id);
	//}

	PatchStatus OperationExecutor::send(const PatchOperation& operation,
		const std::optional<session::SessionInfo>& session_info)
	{
		auto command = operation.get_command(_store.lock(), _request_executor->get_conventions(), _request_executor->get_cache());
		_request_executor->execute(*command, session_info);
		
		if(command->status_code == static_cast<int32_t>(http::HttpStatus::SC_NOT_MODIFIED))
		{
			return PatchStatus::NOT_MODIFIED;
		}
		if(command->status_code == static_cast<int32_t>(http::HttpStatus::SC_NOT_FOUND))
		{
			return PatchStatus::DOCUMENT_DOES_NOT_EXIST;
		}

		return command->get_result()->status;
	}
}
