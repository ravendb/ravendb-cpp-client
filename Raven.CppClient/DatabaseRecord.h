#pragma once
#include "ConflictSolver.h"
#include "DatabaseTopology.h"

namespace  ravendb::client::serverwide
{
	struct DatabaseRecord
	{
		std::string database_name{};
		bool disabled = false;
		std::string data_directory{};
		std::unordered_map<std::string, std::string> settings{};
		ConflictSolver conflict_solver_config{};

		DatabaseTopology topology{};

		std::optional<int64_t> etag{};
	};

	void to_json(nlohmann::json& j, const DatabaseRecord& dbr);

	void from_json(const nlohmann::json& j, DatabaseRecord& dbr);
}
