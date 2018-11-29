#pragma once
#include "stdafx.h"
#include <unordered_set>
#include "IndexPriority.h"
#include "IndexLockMode.h"
#include "IndexType.h"
#include "IndexConfiguration.h"
#include "IndexFieldOptions.h"
#include "json_utils.h"

namespace ravendb::client::documents::indexes
{
	struct IndexDefinition
	{
		 std::string name{};
		 IndexPriority priority{};
		 IndexLockMode lock_mode{};
		 std::unordered_map<std::string, std::string> additional_sources{};
		 std::unordered_set<std::string> maps{};
		 std::string reduce{};
		 std::unordered_map <std::string, IndexFieldOptions> fields{};
		 IndexConfiguration configuration{};
		 IndexType indexType{};
		//TBD 4.1 private boolean testIndex;
		 std::string outputReduceToCollection{};
	};

	inline void to_json(nlohmann::json& j, const IndexDefinition& id)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", id.name);
		//set_val_to_json(j, "Priority", id.priority);
		//set_val_to_json(j, "LockMode", id.lock_mode);
		//set_val_to_json(j, "AdditionalSources", id.additional_sources);
		set_val_to_json(j, "Maps", id.maps);
		//TODO continue !

	}

	inline void from_json(const nlohmann::json& j, IndexDefinition& id)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		//TODO it !
	}


}
