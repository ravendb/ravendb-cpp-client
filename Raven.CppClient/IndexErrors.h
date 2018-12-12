#pragma once

#include "IndexingError.h"
#include "json_utils.h"

namespace ravendb::client::documents::indexes
{
	struct IndexErrors
	{
		std::string name{};
		std::vector<IndexingError> errors{};
	};

	inline void to_json(nlohmann::json& j, const IndexErrors& ie)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", ie.name);
		set_val_to_json(j, "Errors", ie.errors);
	}

	inline void from_json(const nlohmann::json& j, IndexErrors& ie)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", ie.name);
		get_val_from_json(j, "Errors", ie.errors);
	}
}
