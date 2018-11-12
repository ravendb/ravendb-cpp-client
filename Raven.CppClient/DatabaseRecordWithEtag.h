#pragma once
#include "stdafx.h"
#include "DatabaseRecord.h"

namespace  ravendb::client::serverwide
{
	struct DatabaseRecordWithEtag : public DatabaseRecord
	{
		int64_t etag = -1;
	};

	inline void from_json(const nlohmann::json& j, DatabaseRecordWithEtag& p)
	{
		p.etag = j.at("Etag").get<int64_t>();
		p.database_name = j.at("DatabaseName").get<std::string>();
		p.disabled = j.at("Disabled").get<decltype(p.disabled)>();

		//TODO continue
		//p.data_directory = j.at("DataDirectory").get<decltype(p.database_name)>();
		//p.settings = j.at("Settings").get<decltype(p.settings)>();
		//p.conflict_solver_config = j.at("Settings").get<decltype(p.conflict_solver_config)>();
	}
}

