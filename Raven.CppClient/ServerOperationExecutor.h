#pragma once
#include "ClusterRequestExecutor.h"
#include "DocumentStore.h"
#include "IVoidServerOperation.h"

namespace ravendb::client::serverwide::operations
{
	class ServerOperationExecutor
	{
	private:
		std::weak_ptr<ServerOperationExecutor> _weak_this;

		const std::shared_ptr<http::ClusterRequestExecutor> _request_executor;

	private:
		explicit ServerOperationExecutor(std::shared_ptr<documents::DocumentStore> store);

	public:
		~ServerOperationExecutor();

		static std::shared_ptr<ServerOperationExecutor> create(std::shared_ptr<documents::DocumentStore> store);

		void send(const IVoidServerOperation& operation);

		template<typename TResult>
		std::shared_ptr<TResult> send(const IServerOperation<TResult>& operation);

		//TODO
		//public Operation sendAsync(IServerOperation<OperationIdResult> operation) {
		//	RavenCommand<OperationIdResult> command = operation.getCommand(requestExecutor.getConventions());

		//	requestExecutor.execute(command);
		//	return new ServerWideOperation(requestExecutor, requestExecutor.getConventions(), command.getResult().getOperationId());
		//}
		void close();
	};

	template <typename TResult>
	std::shared_ptr<TResult> ServerOperationExecutor::send(const IServerOperation<TResult>& operation)
	{
		auto command = operation.get_command(_request_executor->get_conventions());
		_request_executor->execute(*command);

		return command->get_result();
	}
}
