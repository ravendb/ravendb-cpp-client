#include "stdafx.h"
#include "DatabaseRecord.h"
#include "json_utils.h"

namespace  ravendb::client::serverwide
{
	void to_json(nlohmann::json& j, const DatabaseRecord& dbr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "DatabaseName", dbr.database_name);
		set_val_to_json(j, "Disabled", dbr.disabled);
		set_val_to_json(j, "DataDirectory", dbr.data_directory);
		set_val_to_json(j, "Settings", dbr.settings);
		set_val_to_json(j, "ConflictSolverConfig", dbr.conflict_solver_config);
		set_val_to_json(j, "Topology", dbr.topology);
		set_val_to_json(j, "Etag", dbr.etag);
	}

	void from_json(const nlohmann::json& j, DatabaseRecord& dbr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "DatabaseName", dbr.database_name);
		get_val_from_json(j, "Disabled", dbr.disabled);
		get_val_from_json(j, "DataDirectory", dbr.data_directory);
		get_val_from_json(j, "Settings", dbr.settings);
		get_val_from_json(j, "ConflictSolverConfig", dbr.conflict_solver_config);
		get_val_from_json(j, "Topology", dbr.topology);
		get_val_from_json(j, "Etag", dbr.etag);
	}
}