#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode;


namespace ravendb::client::documents::operations
{
	class GetOperationStateOperation : public operations::IMaintenanceOperation<nlohmann::json>
	{
	private:
		const int64_t _id;
		
	public:
		~GetOperationStateOperation() override = default;

		GetOperationStateOperation(int64_t id)
			: _id(id)
		{}

		std::unique_ptr<RavenCommand<nlohmann::json>> get_command(const DocumentConventions& conventions) const override
		{
			return std::make_unique<GetOperationStateCommand>(DocumentConventions::default_conventions(), _id);
		}

	private:
		class GetOperationStateCommand : public RavenCommand<nlohmann::json>
		{
		private:
			const DocumentConventions _conventions;
			const int64_t _id;

		public:
			~GetOperationStateCommand() override = default;

			GetOperationStateCommand(DocumentConventions conventions, int64_t id)
				: _conventions(std::move(conventions))
				, _id(id)
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases/" << node.database
					<< "/operations/state?id=" << _id;

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = pathBuilder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if(!response.is_null())
				{
					_result = response;
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}
