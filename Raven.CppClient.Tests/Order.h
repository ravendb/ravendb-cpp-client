#pragma once
#include "json_utils.h"
#include "DateTimeOffset.h"
#include "Address.h"
#include "OrderLine.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Order
	{
		std::string id;
		std::string company;
		std::string employee;
		impl::DateTimeOffset ordered_at{};
		impl::DateTimeOffset required_at{};
		impl::DateTimeOffset shipped_at{};
		Address ship_to;
		std::string ship_via;
		double freight{};
		std::vector<OrderLine> lines;
	};

	inline void to_json(nlohmann::json& j, const Order& o)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Id", o.id);
		set_val_to_json(j, "Company", o.company);
		set_val_to_json(j, "Employee", o.employee);
		set_val_to_json(j, "OrderedAt", o.ordered_at);
		set_val_to_json(j, "RequiredAt", o.required_at);
		set_val_to_json(j, "ShippedAt", o.shipped_at);
		set_val_to_json(j, "ShipTo", o.ship_to);
		set_val_to_json(j, "ShipVia", o.ship_via);
		set_val_to_json(j, "Freight", o.freight);
		set_val_to_json(j, "Lines", o.lines);
		//j["@metadata"]["@collection"] = "Orders";
	}

	inline void from_json(const nlohmann::json& j, Order& o)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", o.id);
		get_val_from_json(j, "Company", o.company);
		get_val_from_json(j, "Employee", o.employee);
		get_val_from_json(j, "OrderedAt", o.ordered_at);
		get_val_from_json(j, "RequiredAt", o.required_at);
		get_val_from_json(j, "ShippedAt", o.shipped_at);
		get_val_from_json(j, "ShipTo", o.ship_to);
		get_val_from_json(j, "ShipVia", o.ship_via);
		get_val_from_json(j, "Freight", o.freight);
		get_val_from_json(j, "Lines", o.lines);
	}
}

