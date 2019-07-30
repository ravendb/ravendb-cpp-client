#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Contact
	{
		std::string id;
		std::string firstName;
		std::string surname;
		std::string email;


		friend bool operator==(const Contact& lhs, const Contact& rhs)
		{
			return lhs.firstName == rhs.firstName
				&& lhs.surname == rhs.surname
				&& lhs.email == rhs.email;
		}

		friend bool operator!=(const Contact& lhs, const Contact& rhs)
		{
			return !(lhs == rhs);
		}
	};

	inline void to_json(nlohmann::json& j, const Contact& c)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "firstName", c.firstName);
		set_val_to_json(j, "surname", c.surname);
		set_val_to_json(j, "email", c.email);
	}

	inline void from_json(const nlohmann::json& j, Contact& c)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "firstName", c.firstName);
		get_val_from_json(j, "surname", c.surname);
		get_val_from_json(j, "email", c.email);
	}
}

