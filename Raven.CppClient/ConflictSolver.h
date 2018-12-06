#pragma once
#include "stdafx.h"
#include "ScriptResolver.h"

namespace ravendb::client::serverwide
{
	struct ConflictSolver
	{
		std::map<std::string, ScriptResolver> resolve_by_collection{};
		bool resolve_to_latest = true;
	};

	inline void to_json(nlohmann::json& j, const ConflictSolver& cs)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "ResolveByCollection", cs.resolve_by_collection);
		set_val_to_json(j, "ResolveToLatest", cs.resolve_to_latest);
	}

	inline void from_json(const nlohmann::json& j, ConflictSolver& cs)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "ResolveByCollection", cs.resolve_by_collection);
		get_val_from_json(j, "ResolveToLatest", cs.resolve_to_latest);
	}
}
