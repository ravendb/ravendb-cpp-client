#pragma once

namespace ravendb::client::documents::commands::batches
{
	struct  ReplicationBatchOptions
	{
		bool wait_for_replicas{};
		int32_t number_of_replicas_to_wait_for{};
		std::chrono::milliseconds wait_for_replicas_timeout{};
		bool majority{};
		bool throw_on_timeout_in_wait_for_replicas{};
	};
}
