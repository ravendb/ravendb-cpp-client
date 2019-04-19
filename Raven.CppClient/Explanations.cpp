#include "stdafx.h"
#include "Explanations.h"
#include "QueryResult.h"

namespace ravendb::client::documents::queries::explanation
{
	std::optional<std::vector<std::string>> Explanations::get_explanations(const std::string& key) const
	{
		std::optional<std::vector<std::string>> res{};

		if(explanations)
		{
			if (auto it = explanations->find(key);
				it != explanations->end())
			{
				res.emplace(it->second);
			}
		}

		return res;
	}

	void Explanations::update(const queries::QueryResult& query_result)
	{
		explanations = query_result.explanations;
	}
}
