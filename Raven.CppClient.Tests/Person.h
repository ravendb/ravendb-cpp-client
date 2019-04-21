#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Person
	{
		std::string id;
		std::string name;
		std::string address_id;
	};

	inline void to_json(nlohmann::json& j, const Person& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "name", p.name);
		set_val_to_json(j, "addressId", p.address_id);
	}

	inline void from_json(const nlohmann::json& j, Person& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "name", p.name);
		get_val_from_json(j, "addressId", p.address_id);
	}
}

