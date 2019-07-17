#pragma once
#include "IndexState.h"
#include "IndexLockMode.h"
#include "IndexPriority.h"
#include "IndexType.h"
#include "DateTimeOffset.h"

namespace ravendb::client::documents::operations
{
	struct IndexInformation
	{
		std::string name{};

		bool is_stale{};
		documents::indexes::IndexState state{};
		documents::indexes::IndexLockMode lock_mode{};
		documents::indexes::IndexPriority priority{};
		documents::indexes::IndexType type{};
		impl::DateTimeOffset last_indexing_time{};
	};

	void from_json(const nlohmann::json& j, IndexInformation& ii);
}
