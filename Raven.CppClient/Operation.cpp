#include "stdafx.h"
#include "Operation.h"
#include "RequestExecutor.h"
#include "DocumentConventions.h"
#include "GetOperationStateOperation.h"
#include "OperationCancelledException.h"
#include "OperationExceptionResult.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	std::unique_ptr<http::RavenCommand<nlohmann::json>> Operation::get_operation_state_command(
		std::shared_ptr<conventions::DocumentConventions> conventions, int64_t id)
	{
		return GetOperationStateOperation(id).get_command(conventions);
	}

	Operation::~Operation() = default;

	Operation::Operation(std::shared_ptr<http::RequestExecutor> request_executor,
		std::shared_ptr<conventions::DocumentConventions> conventions, int64_t id)
		: _request_executor(request_executor)
		, _conventions(conventions)
		, _id(id)
	{}

	int64_t Operation::get_id() const
	{
		return _id;
	}

	std::shared_ptr<nlohmann::json> Operation::fetch_operation_status()
	{
		auto command = get_operation_state_command(_conventions, _id);
		_request_executor->execute(*command);

		return command->get_result();
	}

	void Operation::wait_for_completion()
	{
		while(true)
		{
			auto status = fetch_operation_status();

			if(!status)
			{
				throw std::runtime_error("Could not retrieve the operation status from the server");
			}

			std::string operation_status{};
			impl::utils::json_utils::get_val_from_json(*status, "Status", operation_status);

			if(operation_status == "Completed")
			{
				return;
			}

			if(operation_status == "Cancelled")
			{
				throw primitives::OperationCancelledException();
			}

			if(operation_status == "Faulted")
			{
				OperationExceptionResult exception_result{};
				impl::utils::json_utils::get_val_from_json(*status, "Result", exception_result);
				auto schema = exceptions::ExceptionSchema{ _request_executor->get_url().value_or(""),
					exception_result.type, exception_result.message, exception_result.error };

				std::rethrow_exception(exceptions::ExceptionDispatcher::get(schema, exception_result.status_code));
			}
			
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
}
