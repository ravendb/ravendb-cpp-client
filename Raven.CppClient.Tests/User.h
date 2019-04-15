#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct User
	{
		std::string id{};
		std::string name{};
		std::string last_name{};
		std::string address_id{};
		int32_t count{};
		int32_t age{};

		friend bool operator==(const User& lhs, const User& rhs)
		{
			return lhs.id == rhs.id
				&& lhs.name == rhs.name
				&& lhs.last_name == rhs.last_name
				&& lhs.address_id == rhs.address_id
				&& lhs.count == rhs.count
				&& lhs.age == rhs.age;
		}
	};

	inline void to_json(nlohmann::json& j, const User& u)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "id", u.id);
		set_val_to_json(j, "Name", u.name);
		set_val_to_json(j, "LastName", u.last_name);
		set_val_to_json(j, "AddressId", u.address_id);
		set_val_to_json(j, "count", u.count);
		set_val_to_json(j, "Age", u.age);
	}

	inline void from_json(const nlohmann::json& j, User& u)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "id", u.id);
		get_val_from_json(j, "Name", u.name);
		get_val_from_json(j, "LastName", u.last_name);
		get_val_from_json(j, "AddressId", u.address_id);
		get_val_from_json(j, "count", u.count);
		get_val_from_json(j, "Age", u.age);
	}
}

