#pragma once
#include "stdafx.h"
#include "json_utils.h"

namespace  ravendb::client::json
{
	struct BatchCommandResult
	{
		nlohmann::json::array_t results{};
		std::optional<int64_t> transaction_index{};
	};

	inline void from_json(const nlohmann::json& j, BatchCommandResult& bcr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Results", bcr.results);
		get_val_from_json(j, "TransactionIndex", bcr.transaction_index);
	}

	inline void to_json(nlohmann::json& j, const BatchCommandResult& bcr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Results", bcr.results);
		set_val_to_json(j, "TransactionIndex", bcr.transaction_index);
	}
}