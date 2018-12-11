#pragma once
#include "stdafx.h"
#include "ServerNode.h"
#include "json_utils.h"

namespace ravendb::client::http
{
	struct Topology
	{
		std::vector<ServerNode> nodes{};
		int64_t etag = -1;
	};

	inline void from_json(const nlohmann::json& j, Topology& top)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Etag", top.etag);

		auto it = j.find("Nodes");
		if(it == j.end() || ! it->is_array())
		{
			throw std::invalid_argument("\"Nodes\" absent or no array");
		}

		top.nodes.clear();
		top.nodes.reserve(it->size());

		for (const auto& node : *it) 
		{
			top.nodes.emplace_back(node);
		}
	}
}