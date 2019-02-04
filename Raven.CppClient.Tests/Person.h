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

		set_val_to_json(j, "Id", p.id);
		set_val_to_json(j, "Name", p.name);
		set_val_to_json(j, "AddressId", p.address_id);
		j["@metadata"]["@collection"] = "Persons";
	}

	inline void from_json(const nlohmann::json& j, Person& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", p.id);
		get_val_from_json(j, "Name", p.name);
		get_val_from_json(j, "AddressId", p.address_id);
	}
}

