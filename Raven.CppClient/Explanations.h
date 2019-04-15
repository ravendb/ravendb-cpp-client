#pragma once


namespace ravendb::client::documents::queries
{
	struct QueryResult;
}

namespace ravendb::client::documents::queries::explanation
{
	struct Explanations
	{
		std::optional<std::unordered_map<std::string, std::vector<std::string>>> explanations{};

		std::optional<std::vector<std::string>> get_explanations(const std::string& key) const;

		void update(const queries::QueryResult& query_result);
	};
}
