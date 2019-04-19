#pragma once
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	struct CollectionStatistics
	{
		int32_t count_of_documents{};
		int32_t count_of_conflicts{};
		std::unordered_map<std::string, int64_t> collections{};
	};

	inline void from_json(const nlohmann::json& j, CollectionStatistics& cs)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "CountOfDocuments", cs.count_of_documents);
		get_val_from_json(j, "CountOfConflicts", cs.count_of_conflicts);
		get_val_from_json(j, "Collections", cs.collections);
	}
}
