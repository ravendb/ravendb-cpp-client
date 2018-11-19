#pragma once
#include "stdafx.h"

namespace ravendb::client::documents::commands
{
	struct GetDocumentsResult
	{
		nlohmann::json::object_t includes;
		nlohmann::json::array_t results;
		int32_t next_page_start = 0;
	};

	inline void from_json(const nlohmann::json& j, GetDocumentsResult& p)
	{
		p.includes = j.at("Includes").get<nlohmann::json::object_t>();
		p.results = j.at("Results").get<nlohmann::json::array_t>();
		auto next_page_start = j.find("NextPageStart");
		if (next_page_start != j.end())
		{
			p.next_page_start = next_page_start->get<int32_t>();
		}
	}
}