#pragma once
#include "json.hpp"

namespace ravendb::client::serverwide::operations
{
	struct DeleteDatabaseResult
	{
		int64_t raft_command_index = -1;
		std::vector<std::string> pending_deletes{};
	};

	void from_json(const nlohmann::json& j, DeleteDatabaseResult& ddr);
}