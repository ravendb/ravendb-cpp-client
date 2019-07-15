#pragma once
#include "json.hpp"

namespace ravendb::client::documents::queries::explanation
{
	struct ExplanationOptions
	{
		std::string group_key{};
	};

	void to_json(nlohmann::json& j, const ExplanationOptions& eo);
}
