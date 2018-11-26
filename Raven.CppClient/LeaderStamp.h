#pragma once
#include "stdafx.h"
#include "json_utils.h"

namespace  ravendb::client::serverwide
{
	struct LeaderStamp
	{
		int64_t index = -1;
		int64_t term = -1;
		int64_t leaders_ticks = -1;
	};

	inline void from_json(const nlohmann::json& j, LeaderStamp& ls)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Index", ls.index);
		get_val_from_json(j, "Term", ls.term);
		get_val_from_json(j, "LeadersTicks", ls.leaders_ticks);
	}

	inline void to_json(nlohmann::json& j, const LeaderStamp& ls)
	{
		j["Index"] = ls.index;
		j["Term"] = ls.term;
		j["LeadersTicks"] = ls.leaders_ticks;
	}

}