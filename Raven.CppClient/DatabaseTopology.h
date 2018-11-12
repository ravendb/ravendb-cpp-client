#pragma once
#include "stdafx.h"
#include "DatabasePromotionStatus.h"
#include "LeaderStamp.h"

namespace  ravendb::client::serverwide
{
	using ravendb::client::serverwide::operations::DatabasePromotionStatus;

	struct DatabaseTopology
	{
		std::vector<std::string> members{};
		std::vector<std::string> promotables{};
		std::vector<std::string> rehabs{};

		std::unordered_map<std::string, std::string> predefined_mentors{};
		std::unordered_map<std::string, std::string> demotion_reasons{};
		std::unordered_map<std::string, DatabasePromotionStatus> promotables_status{};
		int32_t replication_factor = 1;
		bool dynamic_nodes_distribution = true;
		LeaderStamp stamp{};
	};

	inline void from_json(const nlohmann::json& j, DatabaseTopology& p)
	{
		//TODO !
	}
}