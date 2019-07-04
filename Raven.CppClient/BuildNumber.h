#pragma once
#include "json_utils.h"

namespace ravendb::client::serverwide::operations
{
	struct BuildNumber
	{
		std::string product_version{};
		int32_t build_version{};
		std::string commit_hash{};
		std::string full_version{};
	};

	inline void from_json(const nlohmann::json& j, BuildNumber& bn)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "ProductVersion", bn.product_version);
		get_val_from_json(j, "BuildVersion", bn.build_version);
		get_val_from_json(j, "CommitHash", bn.commit_hash);
		get_val_from_json(j, "FullVersion", bn.full_version);
	}
}
