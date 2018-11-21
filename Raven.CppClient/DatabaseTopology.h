#pragma once
#include "stdafx.h"
#include "DatabasePromotionStatus.h"
#include "LeaderStamp.h"

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

	inline void from_json(const nlohmann::json& j, DatabaseTopology& dbt)
	{
		dbt.members = j.at("Members").get<std::vector<std::string>>();
		dbt.promotables = j.at("Promotables").get<std::vector<std::string>>();
		dbt.rehabs = j.at("Rehabs").get<std::vector<std::string>>();
		if (auto it = j.find("PredefinedMentors"); it != j.end())
		{
			dbt.predefined_mentors = it->get<std::unordered_map<std::string, std::string>>();
		}
		dbt.demotion_reasons = j.at("DemotionReasons").get<std::unordered_map<std::string, std::string>>();

		auto&& temp =
			j.at("PromotablesStatus").get<std::unordered_map<std::string, std::string>>();
		for(const auto& it : temp)
		{
			dbt.promotables_status.emplace(it.first, from_string(it.second));
		}

		dbt.replication_factor = j.at("ReplicationFactor").get<int32_t>();
		dbt.dynamic_nodes_distribution = j.at("DynamicNodesDistribution").get<bool>();
		dbt.stamp = j.at("Stamp").get<LeaderStamp>();
	}
}