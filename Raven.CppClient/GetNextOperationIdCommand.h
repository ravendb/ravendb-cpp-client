#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "ServerNode.h"

using 
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode;

namespace ravendb::client::documents::commands
{
	struct OperationId_t //used with to/from json
	{
		int64_t value;
	};

	inline void from_json(const nlohmann::json& j, OperationId_t& res)
	{
		if (auto&& id = j.find("Id"); id != j.end())
		{
			res.value = id->get<int64_t>();
		}
	}

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
			OperationId_t res = response;
			_result = res.value;
		}

		bool is_read_request() const noexcept override
		{
			return false;// disable caching
		}
	};
}
