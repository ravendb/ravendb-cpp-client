#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Contact
	{
		std::string id;
		std::string first_name;
		std::string surname;
		std::string email;
	};

	inline void to_json(nlohmann::json& j, const Contact& c)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Id", c.id);
		set_val_to_json(j, "FirstName", c.first_name);
		set_val_to_json(j, "Surname", c.surname);
		set_val_to_json(j, "Email", c.email);
	}

	inline void from_json(const nlohmann::json& j, Contact& c)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", c.id);
		get_val_from_json(j, "FirstName", c.first_name);
		get_val_from_json(j, "Surname", c.surname);
		get_val_from_json(j, "Email", c.email);
	}
}

