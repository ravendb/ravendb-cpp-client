#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct User
	{
		std::string id{};
		std::string name{};
		std::string lastName{};
		std::string addressId{};
		int32_t count{};
		int32_t age{};

		friend bool operator==(const User& lhs, const User& rhs)
		{
			return //lhs.id == rhs.id
				   lhs.name == rhs.name
				&& lhs.lastName == rhs.lastName
				&& lhs.addressId == rhs.addressId
				&& lhs.count == rhs.count
				&& lhs.age == rhs.age;
		}
	};

	inline void to_json(nlohmann::json& j, const User& u)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "name", u.name);
		set_val_to_json(j, "lastName", u.lastName);
		set_val_to_json(j, "addressId", u.addressId);
		set_val_to_json(j, "count", u.count);
		set_val_to_json(j, "age", u.age);
	}

	inline void from_json(const nlohmann::json& j, User& u)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "name", u.name);
		get_val_from_json(j, "lastName", u.lastName);
		get_val_from_json(j, "addressId", u.addressId);
		get_val_from_json(j, "count", u.count);
		get_val_from_json(j, "age", u.age);
	}
}

