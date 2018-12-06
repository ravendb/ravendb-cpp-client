#pragma once
#include "IndexQueryBase.h"
#include "Parameters.h"
#include "json_utils.h"

namespace ravendb::client::documents::queries
{
	struct IndexQuery : IndexQueryBase<Parameters>
	{
		~IndexQuery() override = default;

		bool skip_duplicate_checking = false;

		bool disable_caching = false;

		IndexQuery() = default;

		IndexQuery(std::string query_str)
		{
			query = std::move(query_str);
		}
	};

	inline void to_json(nlohmann::json& j, const IndexQuery& id)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Query", id.query);
		set_val_to_json(j, "QueryParameters", id.query_parameters);

		//TODO continue !

	}
}
