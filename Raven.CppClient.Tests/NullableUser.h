#pragma once
#include "json_utils.h"

namespace ravendb::client::tests
{
	struct NullableUser
	{
		std::optional<std::string> id{};
		std::optional<std::string> first_name{};
		std::optional<std::string> last_name{};
		std::optional<std::string> address{};
		std::optional<int32_t> count{};
		std::optional<int32_t> age{};

		friend bool operator==(const NullableUser& lhs, const NullableUser& rhs)
		{
			return lhs.id == rhs.id
				&& lhs.first_name == rhs.first_name
				&& lhs.last_name == rhs.last_name
				&& lhs.address == rhs.address
				&& lhs.count == rhs.count
				&& lhs.age == rhs.age;
		}
	};

	inline void to_json(nlohmann::json& j, const NullableUser& u)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Id", u.id);
		set_val_to_json(j, "FirstName", u.first_name);
		set_val_to_json(j, "LastName", u.last_name);
		set_val_to_json(j, "Address", u.address);
		set_val_to_json(j, "Count", u.count);
		set_val_to_json(j, "Age", u.age);
	}

	inline void from_json(const nlohmann::json& j, NullableUser& u)
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

