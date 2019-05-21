#include "stdafx.h"
#include "SessionOperationExecutor.h"
#include "InMemoryDocumentSessionOperations.h"

namespace ravendb::client::documents::operations
{
	SessionOperationExecutor::SessionOperationExecutor(std::shared_ptr<DocumentStoreBase> store,
		const std::optional<std::string>& database_name)
		: OperationExecutor(store, database_name)
	{}

	SessionOperationExecutor::SessionOperationExecutor(
		std::shared_ptr<session::InMemoryDocumentSessionOperations> session)
		: OperationExecutor(session->get_document_store())
		, _session(session)
	{}

	std::shared_ptr<SessionOperationExecutor> SessionOperationExecutor::create(std::shared_ptr<DocumentStoreBase> store,
		const std::optional<std::string>& database_name)
	{
		auto object = std::shared_ptr<SessionOperationExecutor>(new SessionOperationExecutor(store, database_name));
		object->_weak_this = object;
		return object;
	}

	std::shared_ptr<SessionOperationExecutor> SessionOperationExecutor::create(std::shared_ptr<session::InMemoryDocumentSessionOperations> session)
	{
		auto object = std::shared_ptr<SessionOperationExecutor>(new SessionOperationExecutor(session));
		object->_weak_this = object;
		return object;
	}
}
