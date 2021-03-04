#pragma once
#include "nlohmann/json.hpp"

namespace ravendb::client::extensions
{
	class JsonExtensions
	{
	public:
		static bool try_get_conflict(const nlohmann::json& metadata);
	};
}
