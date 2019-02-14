#include "stdafx.h"
#include "DatabasePutResult.h"
#include "json_utils.h"

namespace  ravendb::client::serverwide::operations
{
	void from_json(const nlohmann::json& j, DatabasePutResult& dpr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "RaftCommandIndex", dpr.raft_command_index);
		get_val_from_json(j, "Name", dpr.name);
		get_val_from_json(j, "Topology", dpr.topology);
		get_val_from_json(j, "NodesAddedTo", dpr.nodes_added_to);
	}
}
