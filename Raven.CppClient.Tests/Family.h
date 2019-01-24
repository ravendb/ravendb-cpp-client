#pragma once
#include "json_utils.h"

namespace ravendb::client::tests
{
	struct Family
	{
		std::vector<std::string> names;

		friend bool operator==(const Family& lhs, const Family& rhs)
		{
			return lhs.names == rhs.names;
		}
	};

	inline void to_json(nlohmann::json& j, const Family& f)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Names", f.names);
		j["@metadata"]["@collection"] = "Families";
	}

	inline void from_json(const nlohmann::json& j, Family& f)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", f.names);
	}
}

