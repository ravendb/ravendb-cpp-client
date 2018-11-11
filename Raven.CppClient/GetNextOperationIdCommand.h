#pragma once
#include "CurlHandlesHolder.h"
#include "RavenCommand.h"
#include "ServerNode.h"

namespace ravendb::client
{
	using http::RavenCommand, http::ServerNode;

	struct OperationId_t
	{
		int64_t value;
	};

	void from_json(const nlohmann::json& j, OperationId_t& p);

	class GetNextOperationIdCommand :public RavenCommand<int64_t>
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
			OperationId_t res = response;
			_result = res.value;
		}

		bool is_read_request() const noexcept override
		{
			return false;// disable caching
		}
	};

	inline void from_json(const nlohmann::json& j, OperationId_t& res)
	{
		if (auto id = j.find("Id"); id not_eq j.end())
		{
			res.value = id->get<int64_t>();
		}
	}

}
