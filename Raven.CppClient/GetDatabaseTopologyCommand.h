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
	private:
		const std::optional<std::string> _debug_tag;

	public:
		~GetDatabaseTopologyCommand() override;

		explicit GetDatabaseTopologyCommand(std::optional<std::string> debug_tag);

		void create_request(CURL* curl, const http::ServerNode& node, std::optional<std::string>& url) override;

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override;

		bool is_read_request() const noexcept override;
	};
}
