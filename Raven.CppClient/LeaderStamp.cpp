#include "stdafx.h"
#include "LeaderStamp.h"
#include "json_utils.h"

namespace  ravendb::client::serverwide
{
	void to_json(nlohmann::json& j, const LeaderStamp& ls)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Index", ls.index);
		set_val_to_json(j, "Term", ls.term);
		set_val_to_json(j, "LeadersTicks", ls.leaders_ticks);
	}

	void from_json(const nlohmann::json& j, LeaderStamp& ls)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Index", ls.index);
		get_val_from_json(j, "Term", ls.term);
		get_val_from_json(j, "LeadersTicks", ls.leaders_ticks);
	}
}