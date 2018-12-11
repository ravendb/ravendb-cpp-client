#pragma once
#include "stdafx.h"

namespace  ravendb::client::documents::commands::batches
{
	struct PutResult
	{
		std::string id{};
		std::string change_vector{};
	};

	inline void from_json(const nlohmann::json& j, PutResult& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", p.id);
		get_val_from_json(j, "ChangeVector", p.change_vector);
	}
}