#pragma once
#include "ScriptResolver.h"

namespace ravendb::client::serverwide
{
	struct ConflictSolver
	{
		std::map<std::string, ScriptResolver> resolve_by_collection{};
		bool resolve_to_latest = true;
	};

	void to_json(nlohmann::json& j, const ConflictSolver& cs);

	void from_json(const nlohmann::json& j, ConflictSolver& cs);
}
