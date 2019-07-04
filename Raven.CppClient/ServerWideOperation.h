#pragma once
#include "Operation.h"
#include "GetServerWideOperationStateOperation.h"

namespace ravendb::client::serverwide::operations
{
	class ServerWideOperation : public documents::operations::Operation
	{
	protected:
		std::unique_ptr<http::RavenCommand<nlohmann::json>> get_operation_state_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions, int64_t id) override
		{
			return GetServerWideOperationStateOperation(id).get_command(conventions);
		}

	public:
		~ServerWideOperation() override = default;

		ServerWideOperation(std::shared_ptr<http::RequestExecutor> request_executor,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions, int64_t id)
			: Operation(request_executor, conventions, id)
		{}
	};
}
