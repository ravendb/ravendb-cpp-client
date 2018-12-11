#pragma once
#include "stdafx.h"
#include "DatabaseTopology.h"

namespace  ravendb::client::serverwide::operations
{
	struct DatabasePutResult
	{
		int64_t raft_command_index = -1;
		std::string name{};
		DatabaseTopology topology{};
		std::vector<std::string> nodes_added_to{};
	};

	inline void from_json(const nlohmann::json& j, DatabasePutResult& dpr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "RaftCommandIndex", dpr.raft_command_index);
		get_val_from_json(j, "Name", dpr.name);
		get_val_from_json(j, "Topology", dpr.topology);
		get_val_from_json(j, "NodesAddedTo", dpr.nodes_added_to);
	}
}
