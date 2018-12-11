#pragma once
#include "json_utils.h"

namespace ravendb::client::documents::indexes
{
	struct PutIndexResult
	{
		std::string index_name;
	};

	inline void to_json(nlohmann::json& j, const PutIndexResult& pir)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Index", pir.index_name);
		
	}

	inline void from_json(const nlohmann::json& j, PutIndexResult& pir)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;
	
		get_val_from_json(j, "Index", pir.index_name);
	}
}
