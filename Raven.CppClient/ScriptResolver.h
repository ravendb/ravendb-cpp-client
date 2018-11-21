#pragma once
#include "stdafx.h"

namespace ravendb::client::serverwide
{
	struct ScriptResolver
	{
		std::string script{};
		std::chrono::system_clock::time_point last_modified_time{};
	};


	inline void to_json(nlohmann::json& j, const ScriptResolver& sr)
	{
		//TODO -> complete
	}
	inline void from_json(const nlohmann::json& j, ScriptResolver& sr)
	{
		//TODO -> complete
	}

}