#include "stdafx.h"
#include "DatabaseTopology.h"
#include "json_utils.h"

namespace  ravendb::client::serverwide
{
	void to_json(nlohmann::json& j, const DatabaseTopology& dbt)
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

	void from_json(const nlohmann::json& j, DatabaseTopology& dbt)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Members", dbt.members);
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