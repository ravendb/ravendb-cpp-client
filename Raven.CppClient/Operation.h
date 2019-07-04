#pragma once
#include <json.hpp>
#include "RavenCommand.h"

namespace ravendb::client
{
	namespace http
	{
		class RequestExecutor;
	}
	namespace documents::conventions
	{
		class DocumentConventions;
	}
}

namespace ravendb::client::documents::operations
{
	class Operation
	{
	private:
		const std::shared_ptr<http::RequestExecutor> _request_executor;
		const std::shared_ptr<conventions::DocumentConventions> _conventions;
		const int64_t _id;

	protected:
		virtual std::unique_ptr<http::RavenCommand<nlohmann::json>> get_operation_state_command(
			std::shared_ptr<conventions::DocumentConventions> conventions, int64_t id);

	public:
		virtual ~Operation();

		Operation(std::shared_ptr<http::RequestExecutor> request_executor,
			std::shared_ptr<conventions::DocumentConventions> conventions,
			int64_t id);

		int64_t get_id() const;

		std::shared_ptr<nlohmann::json> fetch_operation_status();

		void wait_for_completion();
	};
}
