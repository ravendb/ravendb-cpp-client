#pragma once
#include "stdafx.h"
#include "ConflictSolver.h"
#include "json_utils.h"

namespace  ravendb::client::serverwide
{
	struct DatabaseRecord
	{
		std::string database_name{};
		bool disabled = false;
		std::string data_directory{};
		std::unordered_map<std::string, std::string> settings{};
		ConflictSolver conflict_solver_config{};

		std::optional<int64_t> etag{};
	};

	inline void to_json(nlohmann::json& j, const DatabaseRecord& dbr)
	{
		j["DatabaseName"] = dbr.database_name;
		j["Disabled"] = dbr.disabled;
		j["DataDirectory"] = dbr.data_directory;
		j["Settings"] = dbr.settings;
		j["ConflictSolverConfig"] = dbr.conflict_solver_config;

		if(dbr.etag.has_value())
		{
			j["Etag"] = dbr.etag.value();
		}
	}

	inline void from_json(const nlohmann::json& j, DatabaseRecord& dbr)
	{
		dbr.database_name = j.at("DatabaseName").get<std::string>();
		dbr.disabled = j.at("Disabled").get<bool>();
		impl::utils::json_utils::get_val_from_json(j, "DataDirectory", dbr.data_directory);
		dbr.settings = j.at("Settings").get<std::unordered_map<std::string, std::string>>();
		dbr.conflict_solver_config = j.at("ConflictSolverConfig").get<ConflictSolver>();
		dbr.etag = j.at("Etag").get<int64_t>();
	}
}
