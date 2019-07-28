#include "stdafx.h"
#include "JsonExtensions.h"
#include "Constants.h"
#include "json_utils.h"

namespace ravendb::client::extensions
{
	bool JsonExtensions::try_get_conflict(const nlohmann::json& metadata)
	{
		bool has_conflict{ false };

		return impl::utils::json_utils::get_val_from_json(metadata, constants::documents::metadata::CONFLICT, has_conflict)
			&& has_conflict;
	}
}
