#pragma once
#include "OperationExecutor.h"

namespace ravendb::client::documents::session
{
	class InMemoryDocumentSessionOperations;
}

namespace ravendb::client::documents::operations
{
	//For internal session use only
	class SessionOperationExecutor : public OperationExecutor
	{
	private:
		std::weak_ptr<SessionOperationExecutor> _weak_this;
		const std::weak_ptr<session::InMemoryDocumentSessionOperations> _session{};

		SessionOperationExecutor(std::shared_ptr<DocumentStoreBase> store, const std::optional<std::string>& database_name);

		explicit SessionOperationExecutor(std::shared_ptr<session::InMemoryDocumentSessionOperations> session);

	public:
		static std::shared_ptr<SessionOperationExecutor> create(std::shared_ptr<DocumentStoreBase> store,
			const std::optional<std::string>& database_name);

		static std::shared_ptr<SessionOperationExecutor> create(std::shared_ptr<session::InMemoryDocumentSessionOperations> session);
	};
}
