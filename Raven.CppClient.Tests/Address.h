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
		int32_t zip_code;
	};

	inline void to_json(nlohmann::json& j, const Address& a)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Id", a.id);
		set_val_to_json(j, "Country", a.country);
		set_val_to_json(j, "City", a.city);
		set_val_to_json(j, "Street", a.street);
		set_val_to_json(j, "ZipCode", a.zip_code);
	}

	inline void from_json(const nlohmann::json& j, Address& a)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", a.id);
		get_val_from_json(j, "Country", a.country);
		get_val_from_json(j, "City", a.city);
		get_val_from_json(j, "Street", a.street);
		get_val_from_json(j, "ZipCode", a.zip_code);
	}
}

