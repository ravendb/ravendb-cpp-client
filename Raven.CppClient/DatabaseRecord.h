#pragma once
#include "stdafx.h"
#include "ConflictSolver.h"

namespace  ravendb::client::serverwide
{
	struct DatabaseRecord
	{
		std::string database_name{};
		bool disabled = false;
		std::string data_directory{};
		std::unordered_map<std::string, std::string> settings{};
		ConflictSolver conflict_solver_config{};
	};

	inline void to_json(nlohmann::json& j, const DatabaseRecord& dbr)
	{
		j["DatabaseName"] = dbr.database_name;
		j["Disabled"] = dbr.disabled;
		j["Settings"] = dbr.settings;
		//TODO continue
	}

}
