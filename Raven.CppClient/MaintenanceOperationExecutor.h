#pragma once
#include "RequestExecutor.h"
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::documents
{
	class DocumentStore;
}

namespace ravendb::client::documents::operations
{
	class MaintenanceOperationExecutor
	{
	private:
		std::weak_ptr<MaintenanceOperationExecutor> _weak_this{};

		const std::weak_ptr<DocumentStore> _store;
		std::optional<std::string> _database_name;
		std::shared_ptr<http::RequestExecutor> _request_executor{};
		//TODO implement
		//std::shared_ptr< ServerOperationExecutor> _server_operation_executor;

		std::shared_ptr<http::RequestExecutor> get_request_executor();

		void assert_database_set() const;

		MaintenanceOperationExecutor(std::shared_ptr<DocumentStore> store, std::optional<std::string> database_name = {});
	public:
		static std::shared_ptr<MaintenanceOperationExecutor> create(
			std::shared_ptr<DocumentStore> store, std::optional<std::string> database_name = {});

		//TODO implement
		//std::shared_ptr<ServerOperationExecutor> server();

		std::shared_ptr<MaintenanceOperationExecutor> for_database(std::string database_name) const;

		void send(IVoidMaintenanceOperation& operation);

		template<typename TResult>
		std::shared_ptr<TResult> send(const IMaintenanceOperation<TResult>& operation);

		//TODO implement
		//public Operation sendAsync(IMaintenanceOperation<OperationIdResult> operation)
	};

	template <typename TResult>
	std::shared_ptr<TResult> MaintenanceOperationExecutor::send(const IMaintenanceOperation<TResult>& operation)
	{
		assert_database_set();
		auto command = operation.get_command(get_request_executor()->get_conventions());
		get_request_executor()->execute(*command);
		return command->get_result();
		
	}
}
