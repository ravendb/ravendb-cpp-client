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
		const std::reference_wrapper<session::InMemoryDocumentSessionOperations> _session;

	public:
		SessionOperationExecutor(session::InMemoryDocumentSessionOperations& session);
	};
}
