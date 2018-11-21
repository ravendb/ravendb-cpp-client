#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include  "Topology.h"
#include "ServerNode.h"

namespace ravendb::client::serverwide::commands
{
	using ravendb::client::http::RavenCommand,
		ravendb::client::http::Topology,
		ravendb::client::http::ServerNode;

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
			_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return true;
		}
	};
}
