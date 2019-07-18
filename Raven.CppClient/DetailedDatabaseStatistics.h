#pragma once
#include "DatabaseStatistics.h"

namespace ravendb::client::documents::operations
{
	struct DetailedDatabaseStatistics : public DatabaseStatistics
	{
		int64_t count_of_identities{};
		int64_t count_of_compare_exchange{};
	};

	void from_json(const nlohmann::json& j, DetailedDatabaseStatistics& dds);
}

