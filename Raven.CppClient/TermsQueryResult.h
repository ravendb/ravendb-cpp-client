#pragma once
#include "nlohmann/json.hpp"


namespace ravendb::client::documents::queries
{
	struct TermsQueryResult
	{
		std::set<std::string> terms{};
		int64_t result_etag{ -1 };
		std::string index_name{};
	};

	void from_json(const nlohmann::json& j, TermsQueryResult& tqr);
}
