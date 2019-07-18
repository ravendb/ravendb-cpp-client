#pragma once
#include "DateTimeOffset.h"
#include "json.hpp"

namespace ravendb::client::serverwide
{
	struct ScriptResolver
	{
		std::string script{};
		impl::DateTimeOffset last_modified_time{};
	};

	void to_json(nlohmann::json& j, const ScriptResolver& sr);

	void from_json(const nlohmann::json& j, ScriptResolver& sr);
}
