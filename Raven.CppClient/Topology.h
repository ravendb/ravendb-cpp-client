#pragma once
#include "ServerNode.h"

namespace ravendb
{
	struct Topology
	{
		std::vector<ServerNode> nodes;
		int64_t etag = -1;
	};

	inline void from_json(const nlohmann::json& j, Topology& top)
	{
		top.etag = j.at("Etag").get<int64_t>();

		auto&& nodes = j.at("Nodes").get<nlohmann::json::array_t>();
		top.nodes.clear();
		top.nodes.reserve(nodes.size());

		for (auto& node : nodes) 
		{
			top.nodes.emplace(top.nodes.end(), node);
		}
	}
}