#pragma once
#include "CurlHandlesHolder.h"
#include "RavenCommand.h"
#include "ServerNode.h"

namespace ravenDB
{
	using OperationId = int64_t;

	void from_json(const nlohmann::json& j, OperationId& p);

	class GetNextOperationIdCommand :public RavenCommand<OperationId>
	{
	public:

		~GetNextOperationIdCommand() override = default;
		GetNextOperationIdCommand() = default;

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			std::ostringstream urlBuilder;
			urlBuilder << node.url << "/databases/" << node.database
				<< "/operations/next-operation-id";

			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			url = urlBuilder.str();
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return false;// disable caching
		}
	};
}
