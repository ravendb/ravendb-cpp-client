#pragma once
#include "json_utils.h"

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
	j["id"] = u.id;
	j["first_name"] = u.first_name;
	j["last_name"] = u.last_name;
	j["address"] = u.address;
	j["count"] = u.count;
	j["age"] = u.age;
}

inline void from_json(const nlohmann::json& j, User& u)
{
	using ravendb::client::impl::utils::json_utils::get_val_from_json;
	get_val_from_json(j, "id", u.id);
	get_val_from_json(j, "first_name", u.first_name);
	get_val_from_json(j, "last_name", u.last_name);
	get_val_from_json(j, "address", u.address);
	get_val_from_json(j, "count", u.count);
	get_val_from_json(j, "age", u.age);
}

