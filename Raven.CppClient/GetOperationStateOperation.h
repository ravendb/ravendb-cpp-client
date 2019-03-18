#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"

namespace ravendb::client::documents::operations
{
	class GetOperationStateOperation : public operations::IMaintenanceOperation<nlohmann::json>
	{
	private:
		const int64_t _id;
		
	public:
		~GetOperationStateOperation() override = default;

		explicit GetOperationStateOperation(int64_t id)
			: _id(id)
		{}

		std::unique_ptr<RavenCommand<nlohmann::json>> get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<GetOperationStateCommand>(DocumentConventions::default_conventions(), _id);
		}

	private:
		class GetOperationStateCommand : public RavenCommand<nlohmann::json>
		{
		private:
			const std::shared_ptr<DocumentConventions> _conventions;
			const int64_t _id;

		public:
			~GetOperationStateCommand() override = default;

			GetOperationStateCommand(std::shared_ptr<DocumentConventions> conventions, int64_t id)
				: _conventions(conventions)
				, _id(id)
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database
					<< "/operations/state?id=" << _id;

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if(!response.is_null())
				{
					_result = std::make_shared<ResultType>(response.get<ResultType>());
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}
