#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Address
	{
		std::string id;
		std::string country;
		std::string city;
		std::string street;
		int32_t zipCode;
	};

	inline void to_json(nlohmann::json& j, const Address& a)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "country", a.country);
		set_val_to_json(j, "city", a.city);
		set_val_to_json(j, "street", a.street);
		set_val_to_json(j, "zipCode", a.zipCode);
	}

	inline void from_json(const nlohmann::json& j, Address& a)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "country", a.country);
		get_val_from_json(j, "city", a.city);
		get_val_from_json(j, "street", a.street);
		get_val_from_json(j, "zipCode", a.zipCode);
	}
}

