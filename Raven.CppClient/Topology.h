#pragma once
#include "json.hpp"

namespace ravendb::client::http
{
	struct ServerNode;

	struct Topology
	{
		std::vector<ServerNode> nodes{};
		int64_t etag = -1;
	};

	void from_json(const nlohmann::json& j, Topology& top);
}