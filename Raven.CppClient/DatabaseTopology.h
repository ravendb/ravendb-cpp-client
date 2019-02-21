#pragma once
#include "DatabasePromotionStatus.h"
#include "LeaderStamp.h"

namespace  ravendb::client::serverwide
{
	struct DatabaseTopology
	{
		std::vector<std::string> members{};
		std::vector<std::string> promotables{};
		std::vector<std::string> rehabs{};
		std::unordered_map<std::string, std::string> predefined_mentors{};
		std::unordered_map<std::string, std::string> demotion_reasons{};
		std::unordered_map<std::string, operations::DatabasePromotionStatus> promotables_status{};
		int32_t replication_factor = 1;
		bool dynamic_nodes_distribution = false;
		LeaderStamp stamp{};
	};

	void to_json(nlohmann::json& j, const DatabaseTopology& dbt);

	void from_json(const nlohmann::json& j, DatabaseTopology& dbt);
}