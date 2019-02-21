#pragma once
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

	void from_json(const nlohmann::json& j, DatabasePutResult& dpr);
}
