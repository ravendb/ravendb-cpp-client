#include "stdafx.h"
#include "BuildNumber.h"
#include "json_utils.h"

namespace ravendb::client::serverwide::operations
{
	void from_json(const nlohmann::json& j, BuildNumber& bn)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "ProductVersion", bn.product_version);
		get_val_from_json(j, "BuildVersion", bn.build_version);
		get_val_from_json(j, "CommitHash", bn.commit_hash);
		get_val_from_json(j, "FullVersion", bn.full_version);
	}
}
