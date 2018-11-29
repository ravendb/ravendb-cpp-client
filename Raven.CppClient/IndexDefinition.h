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
		 IndexPriority priority = IndexPriority::Normal;
		 IndexLockMode lock_mode;//what default should be ?
		 std::unordered_map<std::string, std::string> additional_sources{};
		 std::unordered_set<std::string> maps{};
		 std::string reduce{};
		 std::unordered_map <std::string, IndexFieldOptions> fields{};
		 IndexConfiguration configuration{};
		 IndexType index_type;//what default should be ?
		 std::string output_reduce_to_collection{};
	};

	inline void to_json(nlohmann::json& j, const IndexDefinition& id)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", id.name);
		//set_val_to_json(j, "Priority", id.priority);
		//set_val_to_json(j, "LockMode", id.lock_mode);
		set_val_to_json(j, "AdditionalSources", id.additional_sources);
		set_val_to_json(j, "Maps", id.maps);


		//TODO continue !

	}

	inline void from_json(const nlohmann::json& j, IndexDefinition& id)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", id.name);
		//get_val_from_json(j, "Priority", id.priority);
		//get_val_from_json(j, "LockMode", id.lock_mode);
		get_val_from_json(j, "AdditionalSources", id.additional_sources);
		get_val_from_json(j, "Maps", id.maps);


		//TODO continue !
	}


}
