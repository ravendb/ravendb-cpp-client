#pragma once

#include "RavenCommand.h"
#include  "Topology.h"
#include "ServerNode.h"

namespace ravendb::client
{
	using http::RavenCommand, http::Topology, http::ServerNode;

	class GetDatabaseTopologyCommand :public RavenCommand<Topology>
	{
	public:

		~GetDatabaseTopologyCommand() override = default;
		GetDatabaseTopologyCommand() = default;

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			std::ostringstream urlBuilder;
			urlBuilder << node.url << "/topology?name=" << node.database;

			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			url = urlBuilder.str();
		}
		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			if (status_code == 200)
			{
				_result = response;
			}
			else
			{
				set_response_not_found(curl);
			}
		}

		bool is_read_request() const noexcept override
		{
			return true;
		}
	};
}
