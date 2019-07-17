#include "stdafx.h"
#include "DetailedDatabaseStatistics.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	void from_json(const nlohmann::json& j, DetailedDatabaseStatistics& dds)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		from_json(j, static_cast<DatabaseStatistics&>(dds));

		get_val_from_json(j, "CountOfIdentities", dds.count_of_identities);
		get_val_from_json(j, "CountOfCompareExchange", dds.count_of_compare_exchange);
	}
}
