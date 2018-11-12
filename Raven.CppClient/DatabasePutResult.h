#pragma once
#include "stdafx.h"
#include "DatabaseTopology.h"

namespace  ravendb::client::serverwide::operations
{
	struct DatabasePutResult
	{
		int64_t raft_command_index{};
		std::string name{};
		DatabaseTopology topology{};
		std::vector<std::string> nodesAddedTo{};
	};

	inline void from_json(const nlohmann::json& j, DatabasePutResult& dpr)
	{
		dpr.raft_command_index = j.at("RaftCommandIndex").get<int64_t>();
		dpr.name = j.at("Name").get<std::string>();
		//dpr.topology = j.at("Topology").get<DatabaseTopology>();
		dpr.nodesAddedTo = j.at("NodesAddedTo").get<std::vector<std::string>>();
	}

}
