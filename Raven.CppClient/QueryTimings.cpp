#include "stdafx.h"
#include "QueryTimings.h"
#include "QueryResult.h"

namespace ravendb::client::documents::queries::timings
{
	void QueryTimings::update(const QueryResult& query_result)
	{
		duration_in_ms = 0;
		timings.reset();

		if(!query_result.timings)
		{
			return;
		}

		duration_in_ms = query_result.timings->duration_in_ms;
		timings = query_result.timings->timings;
	}

	void from_json(const nlohmann::json& j, QueryTimings& qt)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "DurationInMs", qt.duration_in_ms);
		
		if(j.at("Timings").is_null())
		{
			return;
		}

		qt.timings.emplace();

		for(const auto& [key, value] : j.at("Timings").items())
		{
			if(value.is_null())
			{
				qt.timings->insert_or_assign(key, std::optional<QueryTimings>());
			}
			else
			{
				QueryTimings temp{};
				from_json(value, temp);
				qt.timings->insert_or_assign(key, std::move(temp));
			}
		}
	}
}
