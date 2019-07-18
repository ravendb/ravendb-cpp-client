#pragma once
#include "json_utils.h"
#include "IndexRunningStatus.h"

namespace ravendb::client::documents::indexes
{
	struct IndexStatus
	{
		std::string name{};
		IndexRunningStatus status;
	};

	inline void from_json(const nlohmann::json& j, IndexStatus& is)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", is.name);
		get_val_from_json(j, "Status", is.status);
	}

	struct IndexingStatus
	{
		IndexRunningStatus status{};
		std::vector<IndexStatus> indexes{};
	};

	inline void from_json(const nlohmann::json& j, IndexingStatus& is)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Status", is.status);
		get_val_from_json(j, "Indexes", is.indexes);
	}
}
