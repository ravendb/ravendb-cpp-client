#pragma once
#include "stdafx.h"
#include "DatabasePromotionStatus.h"
#include "LeaderStamp.h"
#include "json_utils.h"

namespace  ravendb::client::serverwide
{
	using namespace operations::database_promotion_status;

	struct DatabaseTopology
	{
		std::vector<std::string> members{};
		std::vector<std::string> promotables{};
		std::vector<std::string> rehabs{};

		std::unordered_map<std::string, std::string> predefined_mentors{};
		std::unordered_map<std::string, std::string> demotion_reasons{};
		std::unordered_map<std::string, DatabasePromotionStatus> promotables_status{};
		int32_t replication_factor = 1;
		bool dynamic_nodes_distribution = false;
		LeaderStamp stamp{};
	};

	inline void to_json(nlohmann::json& j, const DatabaseTopology& dbt)
	{
		j["Members"] = dbt.members;
		j["Promotables"] = dbt.promotables;
		j["Rehabs"] = dbt.rehabs;
		j["PredefinedMentors"] = dbt.predefined_mentors;
		j["DemotionReasons"] = dbt.demotion_reasons;
		j["PromotablesStatus"] = dbt.promotables_status;
		j["ReplicationFactor"] = dbt.replication_factor;
		j["DynamicNodesDistribution"] = dbt.dynamic_nodes_distribution;
		j["Stamp"] = dbt.stamp;
	}

	inline void from_json(const nlohmann::json& j, DatabaseTopology& dbt)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j,"Members",dbt.members);
		get_val_from_json(j, "Promotables", dbt.promotables);
		get_val_from_json(j, "Rehabs", dbt.rehabs);
		get_val_from_json(j, "PredefinedMentors", dbt.predefined_mentors);
		get_val_from_json(j, "DemotionReasons", dbt.demotion_reasons);

		std::unordered_map<std::string, std::string> temp{};
		get_val_from_json(j, "PromotablesStatus", temp);
		for(const auto& it : temp)
		{
			dbt.promotables_status.emplace(it.first, from_string(it.second));
		}
		get_val_from_json(j, "ReplicationFactor", dbt.replication_factor);
		get_val_from_json(j, "DynamicNodesDistribution", dbt.dynamic_nodes_distribution);
		get_val_from_json(j, "Stamp", dbt.stamp);
	}
}