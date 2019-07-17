#include "stdafx.h"
#include "IndexInformation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	void from_json(const nlohmann::json& j, IndexInformation& ii)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", ii.name);
		get_val_from_json(j, "IsStale", ii.is_stale);
		get_val_from_json(j, "State", ii.state);
		get_val_from_json(j, "LockMode", ii.lock_mode);
		get_val_from_json(j, "Priority", ii.priority);
		get_val_from_json(j, "LastIndexingTime", ii.last_indexing_time);
	}
}
