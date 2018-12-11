#pragma once
#include "stdafx.h"
#include "IndexPriority.h"
#include "IndexLockMode.h"
#include "IndexType.h"
#include "IndexConfiguration.h"
#include "IndexFieldOptions.h"
#include "json_utils.h"
#include <unordered_set>

namespace ravendb::client::documents::indexes
{
	struct IndexDefinition
	{
		 std::string name{};
		 IndexPriority priority = IndexPriority::UNSET;
		 IndexLockMode lock_mode = IndexLockMode::UNSET;
		 std::unordered_map<std::string, std::string> additional_sources{};
		 std::unordered_set<std::string> maps{};
		 std::string reduce{};
		 std::unordered_map <std::string, IndexFieldOptions> fields{};
		 IndexConfiguration configuration{};
		 IndexType index_type = IndexType::UNSET;
		 std::string output_reduce_to_collection{};
	};

	inline void to_json(nlohmann::json& j, const IndexDefinition& id)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", id.name);
		set_val_to_json(j, "Priority", id.priority);
		set_val_to_json(j, "LockMode", id.lock_mode);
		set_val_to_json(j, "AdditionalSources", id.additional_sources);
		set_val_to_json(j, "Maps", id.maps);
		set_val_to_json(j, "Reduce", id.reduce);
		set_val_to_json(j, "Fields", id.fields);
		set_val_to_json(j, "Configuration", id.configuration);
		set_val_to_json(j, "Type", id.index_type);
		set_val_to_json(j, "OutputReduceToCollection", id.output_reduce_to_collection);
	}

	inline void from_json(const nlohmann::json& j, IndexDefinition& id)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", id.name);
		get_val_from_json(j, "Priority", id.priority);
		get_val_from_json(j, "LockMode", id.lock_mode);
		get_val_from_json(j, "AdditionalSources", id.additional_sources);
		get_val_from_json(j, "Maps", id.maps);
		get_val_from_json(j, "Reduce", id.reduce);
		get_val_from_json(j, "Fields", id.fields);
		get_val_from_json(j, "Configuration", id.configuration);
		get_val_from_json(j, "Type", id.index_type);
		get_val_from_json(j, "OutputReduceToCollection", id.output_reduce_to_collection);
	}
}
