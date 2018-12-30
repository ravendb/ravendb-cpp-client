#pragma once
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	struct OperationIdResult
	{
		int64_t operation_id{};
	};

	inline void from_json(const nlohmann::json& j, OperationIdResult& id)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "OperationId", id.operation_id);
	}
}
