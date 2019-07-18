#pragma once
#include "json.hpp"
#include "ServerNode.h"

namespace  ravendb::client::serverwide
{
	struct CompactSettings
	{
		std::string database_name{};
		std::shared_ptr<const http::ServerNode> node{};
		bool documents{};
		std::vector<std::string> indexes{};
	};

	void to_json(nlohmann::json& j, const CompactSettings& cs);
}
