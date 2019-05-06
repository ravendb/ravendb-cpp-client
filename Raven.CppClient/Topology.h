#pragma once
#include "json_utils.h"

namespace ravendb::client::http
{
	struct ServerNode;

	struct Topology
	{
		std::optional<int64_t> etag{};
		std::shared_ptr<std::vector<std::shared_ptr<ServerNode>>> nodes{};
	};

	void from_json(const nlohmann::json& j, Topology& top);
}