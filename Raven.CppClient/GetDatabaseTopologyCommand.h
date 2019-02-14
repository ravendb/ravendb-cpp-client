#pragma once
#include "RavenCommand.h"
#include "Topology.h"

namespace ravendb::client::http
{
	struct ServerNode;
}

namespace ravendb::client::serverwide::commands
{
	class GetDatabaseTopologyCommand :public http::RavenCommand<http::Topology>
	{
	public:
		~GetDatabaseTopologyCommand() override;

		GetDatabaseTopologyCommand();

		void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override;

		bool is_read_request() const noexcept override;
	};
}
