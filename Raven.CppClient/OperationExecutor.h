#pragma once
#include "IVoidOperation.h"
#include "SessionInfo.h"
#include "RequestExecutor.h"
#include "OperationIdResult.h"
#include "PatchStatus.h"
#include "PatchOperation.h"
#include "Operation.h"

namespace ravendb::client
{
	namespace documents
	{
		class IDocumentStore;
		class DocumentStoreBase;
	}
}


namespace ravendb::client::documents::operations
{
	class OperationExecutor
	{
	private:
		std::weak_ptr<OperationExecutor> _weak_this;

		const std::weak_ptr<IDocumentStore> _store;
		const std::string _database_name;
		const std::shared_ptr<http::RequestExecutor> _request_executor;

	protected:
		explicit OperationExecutor(std::shared_ptr<DocumentStoreBase> store, const std::optional<std::string>& database_name = {});

		explicit OperationExecutor(std::shared_ptr<IDocumentStore> store, const std::optional<std::string>& database_name = {});
		

	public:
		~OperationExecutor() = default;

		static std::shared_ptr<OperationExecutor> create(std::shared_ptr<DocumentStoreBase> store,
			const std::optional<std::string>& database_name = {});

		static std::shared_ptr<OperationExecutor> create(std::shared_ptr<IDocumentStore> store,
			const std::optional<std::string>& database_name = {});

		std::shared_ptr<OperationExecutor> for_database(const std::string& database_name) const;

		void send(IVoidOperation& operation, const std::optional<session::SessionInfo>& session_info = {});

		template<typename TResult>
		std::shared_ptr<TResult> send(IOperation<TResult>& operation, const std::optional<session::SessionInfo>& session_info = {})
		{
			auto command = operation.get_command(_store.lock(), _request_executor->get_conventions(), _request_executor->get_cache());
			_request_executor->execute(*command, session_info);

			return command->get_result();
		}

		std::unique_ptr<Operation> send_async(const IOperation<OperationIdResult>& operation, 
			const std::optional<session::SessionInfo>& session_info = {});

		PatchStatus send(const PatchOperation& operation, const std::optional<session::SessionInfo>& session_info = {});

		template<typename TEntity>
		PatchOperation::Result<TEntity> send(const PatchOperation& operation, const std::optional<session::SessionInfo>& session_info = {})
		{
			auto command = operation.get_command(_store.lock(), _request_executor->get_conventions(), _request_executor->get_cache());
			_request_executor->execute(*command, session_info);

			auto result = PatchOperation::Result<TEntity>{};

			if(command->status_code == static_cast<int32_t>(http::HttpStatus::SC_NOT_MODIFIED))
			{
				result.first = PatchStatus::NOT_MODIFIED;
			}
			if (command->status_code == static_cast<int32_t>(http::HttpStatus::SC_NOT_FOUND))
			{
				result.first = PatchStatus::DOCUMENT_DOES_NOT_EXIST;
			}

			try
			{
				result.first = command->get_result()->status;
				result.second = static_cast<nlohmann::json>(command->get_result()->modified_document).get<TEntity>();
				return result;
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(std::runtime_error(std::string("Unable to read patch result: ") + e.what()));
			}
		}
	};
}
