#pragma once
#include "OperationExecutor.h"

namespace ravendb::client::documents::session
{
	class InMemoryDocumentSessionOperations;
}

namespace ravendb::client::documents::operations
{
	class SessionOperationExecutor : public OperationExecutor
	{
	private:
		const std::shared_ptr<session::InMemoryDocumentSessionOperations> _session;

	public:
		SessionOperationExecutor(std::shared_ptr<session::InMemoryDocumentSessionOperations> session);
	};
}
