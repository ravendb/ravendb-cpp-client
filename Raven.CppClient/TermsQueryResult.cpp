#include "stdafx.h"
#include "TermsQueryResult.h"
#include "json_utils.h"

namespace ravendb::client::documents::queries
{
	void from_json(const nlohmann::json& j, TermsQueryResult& tqr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		std::vector<std::string> temp_terms{};
		get_val_from_json(j, "Terms", temp_terms);
		tqr.terms = std::set<std::string>(temp_terms.cbegin(), temp_terms.cend());

		get_val_from_json(j, "ResultEtag", tqr.result_etag);
		get_val_from_json(j, "IndexName", tqr.index_name);
	}
}
