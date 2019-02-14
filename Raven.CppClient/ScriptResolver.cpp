#include "stdafx.h"
#include "ScriptResolver.h"
#include "json_utils.h"

namespace ravendb::client::serverwide
{
	void to_json(nlohmann::json& j, const ScriptResolver& sr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Script", sr.script);
		set_val_to_json(j, "LastModifiedTime", sr.last_modified_time);
	}

	void from_json(const nlohmann::json& j, ScriptResolver& sr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Script", sr.script);
		get_val_from_json(j, "LastModifiedTime", sr.last_modified_time);
	}
}