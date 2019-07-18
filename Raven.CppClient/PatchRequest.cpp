#include "stdafx.h"
#include "PatchRequest.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	PatchRequest::PatchRequest(std::string script)
		: script(std::move(script))
	{}

	void to_json(nlohmann::json& j, const PatchRequest& pr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Script", pr.script);
		set_val_to_json(j, "Values", pr.values);
	}
}
