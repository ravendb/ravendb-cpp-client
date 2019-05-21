#pragma once

namespace ravendb::client::serverwide
{
	struct LeaderStamp
	{
		int64_t index = -1;
		int64_t term = -1;
		int64_t leaders_ticks = -1;
	};

	void to_json(nlohmann::json& j, const LeaderStamp& ls);

	void from_json(const nlohmann::json& j, LeaderStamp& ls);
}