#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Employee
	{
		std::string id;
		std::string first_name;
		std::string last_name;
	};

	inline void to_json(nlohmann::json& j, const Employee& e)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Id", e.id);
		set_val_to_json(j, "FirstName", e.first_name);
		set_val_to_json(j, "LastName", e.last_name);
		j["@metadata"]["@collection"] = "Employees";
	}

	inline void from_json(const nlohmann::json& j, Employee& e)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", e.id);
		get_val_from_json(j, "FirstName", e.first_name);
		get_val_from_json(j, "LastName", e.last_name);
	}
}

