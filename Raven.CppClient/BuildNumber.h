#pragma once
#include "nlohmann/json.hpp"


namespace ravendb::client::serverwide::operations
{
	struct BuildNumber
	{
		std::string product_version{};
		int32_t build_version{};
		std::string commit_hash{};
		std::string full_version{};
	};

	void from_json(const nlohmann::json& j, BuildNumber& bn);
}
