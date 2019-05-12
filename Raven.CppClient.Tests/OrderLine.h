#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct OrderLine
	{
		std::string product;
		std::string productName;
		double pricePerUnit{};
		int32_t quantity{};
		double discount{};
	};

	inline void to_json(nlohmann::json& j, const OrderLine& ol)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "product", ol.product);
		set_val_to_json(j, "productName", ol.productName);
		set_val_to_json(j, "pricePerUnit", ol.pricePerUnit);
		set_val_to_json(j, "quantity", ol.quantity);
		set_val_to_json(j, "discount", ol.discount);
	}

	inline void from_json(const nlohmann::json& j, OrderLine& ol)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "product", ol.product);
		get_val_from_json(j, "productName", ol.productName);
		get_val_from_json(j, "pricePerUnit", ol.pricePerUnit);
		get_val_from_json(j, "quantity", ol.quantity);
		get_val_from_json(j, "discount", ol.discount);
	}
}

