#pragma once
#include <chrono>

namespace ravendb::client::documents::commands::batches
{
	struct  IndexBatchOptions
	{
		bool wait_for_indexes{};
		std::chrono::milliseconds wait_for_indexes_timeout{};
		bool throw_on_timeout_in_wait_for_indexes{};
		std::vector<std::string> wait_for_specific_indexes{};
	};
}
