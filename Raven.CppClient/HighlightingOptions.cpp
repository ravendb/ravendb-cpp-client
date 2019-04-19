#include "stdafx.h"
#include "HighlightingOptions.h"
#include "json_utils.h"

namespace ravendb::client::documents::queries::highlighting
{
	void to_json(nlohmann::json& j, const HighlightingOptions& ho)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "GroupKey", ho.group_key);
		set_val_to_json(j, "PreTags", ho.pre_tags);
		set_val_to_json(j, "PostTags", ho.post_tags);
	}
}
