#include "stdafx.h"
#include "IndexQuery.h"
#include "xxhash.h"

namespace ravendb::client::documents::queries
{
	IndexQuery::IndexQuery(std::string query_str)
	{
		query = std::move(query_str);
	}

	std::string IndexQuery::get_query_hash() const
	{
		XXH64_state_t* const state = XXH64_createState();
		XXH64_update(state, nlohmann::json(*this).dump().c_str(), nlohmann::json(*this).dump().size());
		return std::to_string(XXH64_digest(state));
	}

	void to_json(nlohmann::json& j, const IndexQuery& iq)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		to_json(j, static_cast<const IndexQueryBase<Parameters>&>(iq));

		set_val_to_json(j, "SkipDuplicateChecking", iq.skip_duplicate_checking);
		set_val_to_json(j, "ExplainScores", iq.explain_scores);
		set_val_to_json(j, "ShowTimings", iq.show_timings);
		set_val_to_json(j, "DisableCaching", iq.disable_caching);
	}
}
