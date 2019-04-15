#pragma once

namespace ravendb::client::documents::queries
{
	struct QueryResult;
}

namespace ravendb::client::documents::queries::timings
{
	struct QueryTimings
	{
		int64_t duration_in_ms{};
		std::optional<std::map<std::string, std::optional<QueryTimings>>> timings{};

		void update(const QueryResult& query_result);
	};

	void from_json(const nlohmann::json& j, QueryTimings& qt);
}
