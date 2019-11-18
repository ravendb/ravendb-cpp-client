#pragma once
#include <string>
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::operations
{
	struct DisableDatabaseToggleResult
	{
		bool disabled{};
		std::string name{};
		bool success{};
		std::string reason{};
	};

	void from_json(const nlohmann::json& j, DisableDatabaseToggleResult& ddtr);
}

