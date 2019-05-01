#include "stdafx.h"
#include "GetDatabaseTopologyCommand.h"
#include "ServerNode.h"

namespace ravendb::client::serverwide::commands
{
	GetDatabaseTopologyCommand::~GetDatabaseTopologyCommand() = default;

	GetDatabaseTopologyCommand::GetDatabaseTopologyCommand(std::optional<std::string> debug_tag)
		: _debug_tag(std::move(debug_tag))
	{}

	void GetDatabaseTopologyCommand::create_request(CURL * curl, const http::ServerNode & node, std::optional<std::string> & url)
	{
		std::ostringstream url_builder;
		url_builder << node.url << "/topology?name=" << node.database;

		if(_debug_tag)
		{
			url_builder << "&" << *_debug_tag;
		}

		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

		url.emplace(url_builder.str());
	}

	void GetDatabaseTopologyCommand::set_response(CURL * curl, const nlohmann::json & response, bool from_cache)
	{
		_result = std::make_shared<ResultType>(response.get<ResultType>());
	}

	bool GetDatabaseTopologyCommand::is_read_request() const noexcept
	{
		return true;
	}
}