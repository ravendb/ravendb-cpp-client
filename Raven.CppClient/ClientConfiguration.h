#pragma once
#include "ReadBalanceBehavior.h"

namespace ravendb::client::documents::operations::configuration
{
	struct ClientConfiguration
	{
		int64_t etag{};
		bool disabled = false;
		std::optional<int32_t> max_number_of_requests_per_session{};
		http::ReadBalanceBehavior read_balance_behaviour{};
	};
}
