#pragma once
#include "stdafx.h"

namespace ravendb::client::serverwide::operations
{
	struct DeleteDatabaseResult
	{
		int64_t raft_command_index{};
		std::vector<std::string> pending_deletes{};
	};

	inline void from_json(const nlohmann::json& j, DeleteDatabaseResult& ddr)
	{
		ddr.raft_command_index = j.at("RaftCommandIndex").get<int64_t>();
		ddr.pending_deletes = j.at("PendingDeletes").get<std::vector<std::string>>();
	}
}