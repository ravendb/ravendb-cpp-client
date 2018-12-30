#pragma once
#include "json_utils.h"

namespace ravendb::client::tests
{
	struct User
	{
		std::string id;
		std::string first_name;
		std::string last_name;
		std::string address;
		int32_t count;
		int32_t age;

		friend bool operator==(const User& lhs, const User& rhs)
		{
			return lhs.id == rhs.id
				&& lhs.first_name == rhs.first_name
				&& lhs.last_name == rhs.last_name
				&& lhs.address == rhs.address
				&& lhs.count == rhs.count
				&& lhs.age == rhs.age;
		}
	};

	inline void to_json(nlohmann::json& j, const User& u)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Id", u.id);
		set_val_to_json(j, "FirstName", u.first_name);
		set_val_to_json(j, "LastName", u.last_name);
		set_val_to_json(j, "Address", u.address);
		set_val_to_json(j, "Count", u.count);
		set_val_to_json(j, "Age", u.age);
		j["@metadata"]["@collection"] = "Users";
	}

	inline void from_json(const nlohmann::json& j, User& u)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", u.id);
		get_val_from_json(j, "FirstName", u.first_name);
		get_val_from_json(j, "LastName", u.last_name);
		get_val_from_json(j, "Address", u.address);
		get_val_from_json(j, "Count", u.count);
		get_val_from_json(j, "Age", u.age);
	}
}

