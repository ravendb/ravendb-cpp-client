#pragma once
#include "stdafx.h"
#include "json_utils.h"


namespace ravendb::client::documents::commands
{
	struct GetDocumentsResult
	{
		nlohmann::json::object_t includes;
		nlohmann::json::array_t results;
		int32_t next_page_start = 0;
	};

	inline void from_json(const nlohmann::json& j, GetDocumentsResult& gdr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Includes", gdr.includes);
		get_val_from_json(j, "Results", gdr.results);
		get_val_from_json(j, "NextPageStart", gdr.results);
	}
}