#include "stdafx.h"
#include "ExplanationOptions.h"
#include "json_utils.h"

namespace ravendb::client::documents::queries::explanation
{
	void to_json(nlohmann::json& j, const ExplanationOptions& eo)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "GroupKey", eo.group_key);
	}
}
