#pragma once

namespace ravendb::client::documents::queries
{
	struct 	QueryOperationOptions
	{
		std::optional<int32_t> max_ops_per_second{};
		bool allow_stale;
		std::optional<std::chrono::milliseconds> stale_timeout{};
		bool retrieve_details;

		explicit QueryOperationOptions(std::optional<int32_t> max_ops_per_sec_ = {}, bool allow_stale_ = false,
			std::optional<std::chrono::milliseconds> stale_timeout_ = {}, bool retrieve_details_ = false)
			: max_ops_per_second([&]
		{
			if(max_ops_per_sec_.has_value() && *max_ops_per_sec_ <= 0)
			{
				throw std::invalid_argument("max_ops_per_sec_ must be greater than 0");
			}else
			{
				return std::move(max_ops_per_sec_);
			}
		}())
			, allow_stale(allow_stale_)
			, stale_timeout(std::move(stale_timeout_))
			, retrieve_details(retrieve_details_)
		{}
	};
}
