#pragma once
#include <optional>
#include "ReplicationBatchOptions.h"
#include "IndexBatchOptions.h"

namespace ravendb::client::documents::commands::batches
{
	struct  BatchOptions
	{
		std::optional<ReplicationBatchOptions> replication_options{};
		std::optional<IndexBatchOptions> index_options{};
	};
}
