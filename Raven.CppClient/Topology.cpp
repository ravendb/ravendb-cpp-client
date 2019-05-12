#include "stdafx.h"
#include "Topology.h"
#include "ServerNode.h"
#include "json_utils.h"

namespace ravendb::client::http
{
	void from_json(const nlohmann::json & j, Topology & top)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Etag", top.etag);

		top.nodes = std::make_shared<std::vector<std::shared_ptr<ServerNode>>>();
		top.nodes->reserve(j.at("Nodes").size());
		for(const auto& json_node : j.at("Nodes"))
		{
			auto node = std::make_shared<ServerNode>(json_node.get<ServerNode>());
			top.nodes->emplace_back(node);
		}
	}
}