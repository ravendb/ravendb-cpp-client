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
		impl::DateTimeOffset orderedAt{};
		impl::DateTimeOffset requiredAt{};
		impl::DateTimeOffset shippedAt{};
		Address shipTo;
		std::string shipVia;
		double freight{};
		std::vector<OrderLine> lines;
	};

	inline void to_json(nlohmann::json& j, const Order& o)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "company", o.company);
		set_val_to_json(j, "employee", o.employee);
		set_val_to_json(j, "orderedAt", o.orderedAt);
		set_val_to_json(j, "requiredAt", o.requiredAt);
		set_val_to_json(j, "shippedAt", o.shippedAt);
		set_val_to_json(j, "shipTo", o.shipTo);
		set_val_to_json(j, "shipVia", o.shipVia);
		set_val_to_json(j, "freight", o.freight);
		set_val_to_json(j, "lines", o.lines);
	}

	inline void from_json(const nlohmann::json& j, Order& o)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "company", o.company);
		get_val_from_json(j, "employee", o.employee);
		get_val_from_json(j, "orderedAt", o.orderedAt);
		get_val_from_json(j, "requiredAt", o.requiredAt);
		get_val_from_json(j, "shippedAt", o.shippedAt);
		get_val_from_json(j, "shipTo", o.shipTo);
		get_val_from_json(j, "shipVia", o.shipVia);
		get_val_from_json(j, "freight", o.freight);
		get_val_from_json(j, "lines", o.lines);
	}
}

