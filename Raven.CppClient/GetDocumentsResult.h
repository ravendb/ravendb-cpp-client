#pragma once
#include "json.hpp"

namespace ravendb::client::documents::commands
{
	struct GetDocumentsResult
	{
		nlohmann::json::object_t includes{};
		nlohmann::json::array_t results{};
		int32_t next_page_start{ 0 };
	};

	void from_json(const nlohmann::json& j, GetDocumentsResult& gdr);
}