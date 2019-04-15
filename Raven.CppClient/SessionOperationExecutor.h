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
		std::weak_ptr<SessionOperationExecutor> _weak_this;
		const std::weak_ptr<session::InMemoryDocumentSessionOperations> _session;

		SessionOperationExecutor(std::shared_ptr<session::InMemoryDocumentSessionOperations> session);
	public:
		static std::shared_ptr<SessionOperationExecutor> create(std::shared_ptr<session::InMemoryDocumentSessionOperations> session);
	};
}
