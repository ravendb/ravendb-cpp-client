#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "ServerNode.h"

using 
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode;

namespace ravendb::client::documents::commands
{

	class GetNextOperationIdCommand :public RavenCommand<int64_t>
	{
	public:

		~GetNextOperationIdCommand() override = default;
		GetNextOperationIdCommand() = default;

		void create_request(CURL* curl, const ServerNode& node, std::string& url) override
		{
			std::ostringstream urlBuilder;
			urlBuilder << node.url << "/databases/" << node.database
				<< "/operations/next-operation-id";

			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			url = urlBuilder.str();
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			_result = std::make_shared<ResultType>();
			if(! impl::utils::json_utils::get_val_from_json(response, "Id", *_result))
			{
				throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::INVALID_RESPONSE);
			}
			
		}

		bool is_read_request() const noexcept override
		{
			return false;// disable caching
		}
	};
}
