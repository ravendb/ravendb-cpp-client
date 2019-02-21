#include "stdafx.h"
#include "GetDatabaseTopologyCommand.h"
#include "ServerNode.h"

namespace ravendb::client::serverwide::commands
{
	GetDatabaseTopologyCommand::~GetDatabaseTopologyCommand() = default;

	GetDatabaseTopologyCommand::GetDatabaseTopologyCommand() = default;

	void GetDatabaseTopologyCommand::create_request(CURL * curl, const http::ServerNode & node, std::string & url)
	{
		std::ostringstream urlBuilder;
		urlBuilder << node.url << "/topology?name=" << node.database;

		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

		url = urlBuilder.str();
	}

	void GetDatabaseTopologyCommand::set_response(CURL * curl, const nlohmann::json & response, bool from_cache)
	{
		_result = response.get<decltype(_result)>();
	}

	bool GetDatabaseTopologyCommand::is_read_request() const noexcept
	{
		return true;
	}
}