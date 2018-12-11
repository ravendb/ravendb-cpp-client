#pragma once
#include "stdafx.h"
#include "DatabasePromotionStatus.h"
#include "LeaderStamp.h"
#include "json_utils.h"

using
	ravendb::client::serverwide::operations::DatabasePromotionStatus;

namespace  ravendb::client::serverwide
{
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
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Members", dbt.members);
		set_val_to_json(j, "Promotables", dbt.promotables);
		set_val_to_json(j, "Rehabs", dbt.rehabs);
		set_val_to_json(j, "PredefinedMentors", dbt.predefined_mentors);
		set_val_to_json(j, "DemotionReasons", dbt.demotion_reasons);
		set_val_to_json(j, "PromotablesStatus", dbt.promotables_status);
		set_val_to_json(j, "ReplicationFactor", dbt.replication_factor);
		set_val_to_json(j, "DynamicNodesDistribution", dbt.dynamic_nodes_distribution);
		set_val_to_json(j, "Stamp", dbt.stamp);
	}

	inline void from_json(const nlohmann::json& j, DatabaseTopology& dbt)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j,"Members",dbt.members);
		get_val_from_json(j, "Promotables", dbt.promotables);
		get_val_from_json(j, "Rehabs", dbt.rehabs);
		get_val_from_json(j, "PredefinedMentors", dbt.predefined_mentors);
		get_val_from_json(j, "DemotionReasons", dbt.demotion_reasons);
		get_val_from_json(j, "PromotablesStatus", dbt.promotables_status);
		get_val_from_json(j, "ReplicationFactor", dbt.replication_factor);
		get_val_from_json(j, "DynamicNodesDistribution", dbt.dynamic_nodes_distribution);
		get_val_from_json(j, "Stamp", dbt.stamp);
	}
}