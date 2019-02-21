#include "stdafx.h"
#include "ConflictSolver.h"
#include "json_utils.h"

namespace ravendb::client::serverwide
{
	void to_json(nlohmann::json& j, const ConflictSolver& cs)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "ResolveByCollection", cs.resolve_by_collection);
		set_val_to_json(j, "ResolveToLatest", cs.resolve_to_latest);
	}

	void from_json(const nlohmann::json& j, ConflictSolver& cs)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "ResolveByCollection", cs.resolve_by_collection);
		get_val_from_json(j, "ResolveToLatest", cs.resolve_to_latest);
	}
}