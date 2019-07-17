#pragma once
#include <optional>
#include "chrono"

namespace ravendb::client::documents::queries
{
	struct 	QueryOperationOptions
	{
		std::optional<int32_t> max_ops_per_second{};
		bool allow_stale{};
		std::optional<std::chrono::milliseconds> stale_timeout{};
		bool retrieve_details{};

		explicit QueryOperationOptions(std::optional<int32_t> max_ops_per_sec_ = {}, bool allow_stale_ = false,
			std::optional<std::chrono::milliseconds> stale_timeout_ = {}, bool retrieve_details_ = false);
	};
}
