#include "stdafx.h"
#include "DeleteDatabaseResult.h"
#include "json_utils.h"

namespace ravendb::client::serverwide::operations
{
	void from_json(const nlohmann::json& j, DeleteDatabaseResult& ddr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "RaftCommandIndex", ddr.raft_command_index);
		get_val_from_json(j, "PendingDeletes", ddr.pending_deletes);
	}
}
