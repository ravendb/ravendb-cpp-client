#pragma once
#include "stdafx.h"

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
		ls.index = j.at("Index").get<int64_t>();
		ls.term = j.at("Term").get<int64_t>();
		ls.leaders_ticks = j.at("LeadersTicks").get<int64_t>();
	}
}