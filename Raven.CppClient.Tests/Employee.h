#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Employee
	{
		std::string id;
		std::string firstName;
		std::string lastName;
	};

	inline void to_json(nlohmann::json& j, const Employee& e)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "firstName", e.firstName);
		set_val_to_json(j, "lastName", e.lastName);
	}

	inline void from_json(const nlohmann::json& j, Employee& e)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "firstName", e.firstName);
		get_val_from_json(j, "lastName", e.lastName);
	}
}

