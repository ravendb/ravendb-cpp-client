#pragma once
#include "json.hpp"

namespace ravendb::client::documents::operations
{
	struct PatchRequest
	{
		std::string script{};
		std::unordered_map<std::string, nlohmann::json> values{};

		PatchRequest() = default;

		explicit PatchRequest(std::string script);
	};

	void to_json(nlohmann::json& j, const PatchRequest& pr);
}
