#include "stdafx.h"
#include "IndexQuery.h"
#include "xxhash.hpp"

namespace ravendb::client::documents::queries
{
	IndexQuery::IndexQuery(std::string query_str)
	{
		query = std::move(query_str);
	}

	std::string IndexQuery::get_query_hash() const
	{
		xxh::hash_state_t<64> hash_stream;

		hash_stream.update(nlohmann::json(*this).dump());

		return std::to_string(hash_stream.digest());
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
