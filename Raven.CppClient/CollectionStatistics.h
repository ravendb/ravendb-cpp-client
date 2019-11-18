#pragma once
#include "nlohmann/json.hpp"


namespace ravendb::client::documents::operations
{
	struct CollectionStatistics
	{
		int32_t count_of_documents{};
		int32_t count_of_conflicts{};
		std::unordered_map<std::string, int64_t> collections{};
	};

	void from_json(const nlohmann::json& j, CollectionStatistics& cs);
}
