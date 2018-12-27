#pragma once
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	struct PatchRequest
	{
		std::string script{};
		std::unordered_map<std::string, nlohmann::json> values{};

		explicit PatchRequest(std::string script)
			: script(std::move(script))
		{}
	};

	inline void to_json(nlohmann::json& j, const PatchRequest& pr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Script", pr.script);
		set_val_to_json(j, "Values", pr.values);
	}
}
