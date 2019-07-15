#pragma once
#include "json.hpp"

namespace  ravendb::client::json
{
	struct BatchCommandResult
	{
		nlohmann::json::array_t results{};
		std::optional<int64_t> transaction_index{};
	};

	void from_json(const nlohmann::json& j, BatchCommandResult& bcr);

	void to_json(nlohmann::json& j, const BatchCommandResult& bcr);
}