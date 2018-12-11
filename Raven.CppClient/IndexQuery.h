#pragma once
#include "IndexQueryBase.h"
#include "Parameters.h"

namespace ravendb::client::documents::queries
{
	struct IndexQuery : IndexQueryBase<Parameters>
	{
		~IndexQuery() override = default;

		bool skip_duplicate_checking = false;

		bool explain_scores = false;

		bool show_timings = false;

		bool disable_caching = false;

		IndexQuery() = default;

		IndexQuery(std::string query_str)
		{
			query = std::move(query_str);
		}
	};

	inline void to_json(nlohmann::json& j, const IndexQuery& iq)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		to_json(j, static_cast<const IndexQueryBase<Parameters>&>(iq));

		set_val_to_json(j, "SkipDuplicateChecking", iq.skip_duplicate_checking);
		set_val_to_json(j, "ExplainScores", iq.explain_scores);
		set_val_to_json(j, "ShowTimings", iq.show_timings);
		set_val_to_json(j, "DisableCaching", iq.disable_caching);
	}
}
