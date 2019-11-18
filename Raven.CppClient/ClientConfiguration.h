#pragma once
#include "ReadBalanceBehavior.h"
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::operations::configuration
{
	struct ClientConfiguration
	{
		int64_t etag{};
		bool disabled = false;
		std::optional<int32_t> max_number_of_requests_per_session{};
		std::optional<http::ReadBalanceBehavior> read_balance_behaviour{};
	};

	void from_json(const nlohmann::json& j, ClientConfiguration& cc);

	void to_json(nlohmann::json& j, const ClientConfiguration& cc);
}
