#pragma once
#include "stdafx.h"
#include "GenericQueryResult.h"

namespace ravendb::client::documents::queries
{
	struct QueryResult : GenericQueryResult<nlohmann::json::array_t, nlohmann::json::object_t>
	{
		~QueryResult() override = default;

		QueryResult() = default;
		QueryResult(const QueryResult& other) = default;//TODO check if OK

		QueryResult create_snapshot() const
		{
			return { *this };
		}

	};

	inline void from_json(const nlohmann::json& j, QueryResult& qr)
	{
		from_json(j, static_cast<GenericQueryResult<nlohmann::json::array_t, nlohmann::json::object_t>&>(qr));
	}

}
