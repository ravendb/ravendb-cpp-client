#include "stdafx.h"
#include "DisableDatabaseToggleResult.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	void from_json(const nlohmann::json& j, DisableDatabaseToggleResult& ddtr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Disabled", ddtr.disabled);
		get_val_from_json(j, "Name", ddtr.name);
		get_val_from_json(j, "Success", ddtr.success);
		get_val_from_json(j, "Reason", ddtr.reason);
	}
}
