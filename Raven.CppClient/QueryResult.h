#pragma once
#include "GenericQueryResult.h"

namespace ravendb::client::documents::queries
{
	struct QueryResult : GenericQueryResult<nlohmann::json::array_t, nlohmann::json::object_t>
	{
		~QueryResult() override = default;

		QueryResult() = default;

		QueryResult create_snapshot() const
		{
			return *this ;
		}

	};

	inline void from_json(const nlohmann::json& j, QueryResult& qr)
	{
		from_json(j, static_cast<GenericQueryResult<nlohmann::json::array_t, nlohmann::json::object_t>&>(qr));
	}

}
