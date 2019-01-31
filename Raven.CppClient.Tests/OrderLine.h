#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct OrderLine
	{
		std::string product;
		std::string product_name;
		double price_per_unit{};
		uint32_t quantity{};
		double discount{};
	};

	inline void to_json(nlohmann::json& j, const OrderLine& ol)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Product", ol.product);
		set_val_to_json(j, "ProductName", ol.product_name);
		set_val_to_json(j, "PricePerUnit", ol.price_per_unit);
		set_val_to_json(j, "Quantity", ol.quantity);
		set_val_to_json(j, "Discount", ol.discount);
	}

	inline void from_json(const nlohmann::json& j, OrderLine& ol)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Product", ol.product);
		get_val_from_json(j, "ProductName", ol.product_name);
		get_val_from_json(j, "PricePerUnit", ol.price_per_unit);
		get_val_from_json(j, "Quantity", ol.quantity);
		get_val_from_json(j, "Discount", ol.discount);
	}
}

