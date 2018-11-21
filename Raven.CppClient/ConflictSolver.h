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
		j["ResolveByCollection"] = cs.resolve_by_collection;
		j["ResolveToLatest"] = cs.resolve_to_latest;
	}

	inline void from_json(const nlohmann::json& j, ConflictSolver& cs)
	{
		cs.resolve_by_collection = j.at("ResolveByCollection").get<std::map<std::string, ScriptResolver>>();
		cs.resolve_to_latest = j.at("ResolveToLatest").get<bool>();
	}
}
