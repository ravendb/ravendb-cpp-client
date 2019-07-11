#pragma once
#include "json.hpp"

namespace ravendb::client::documents::queries::highlighting
{
	struct HighlightingOptions
	{
		std::string group_key{};
		std::vector<std::string> pre_tags{};
		std::vector<std::string> post_tags{};
	};

	void to_json(nlohmann::json& j, const HighlightingOptions& ho);
}
