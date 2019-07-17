#include "stdafx.h"
#include "QueryOperationOptions.h"

namespace ravendb::client::documents::queries
{
	QueryOperationOptions::QueryOperationOptions(std::optional<int32_t> max_ops_per_sec_, bool allow_stale_,
		std::optional<std::chrono::milliseconds> stale_timeout_, bool retrieve_details_)
		: max_ops_per_second([&]
	{
		if (max_ops_per_sec_.has_value() && *max_ops_per_sec_ <= 0)
		{
			throw std::invalid_argument("max_ops_per_sec_ must be greater than 0");
		}
		return max_ops_per_sec_;
	}())
		, allow_stale(allow_stale_)
		, stale_timeout(stale_timeout_)
		, retrieve_details(retrieve_details_)
	{}

}
