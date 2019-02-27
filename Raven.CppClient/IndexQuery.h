#pragma once
#include "IndexQueryBase.h"
#include "Parameters.h"
//TODO put in final project
#include "C:\work\xxhash_cpp\xxhash\xxhash.hpp"

namespace ravendb::client::documents::queries
{
	struct IndexQuery : IndexQueryBase<Parameters>
	{
		bool skip_duplicate_checking = false;

		bool explain_scores = false;

		bool show_timings = false;

		bool disable_caching = false;

		IndexQuery() = default;

		explicit IndexQuery(std::string query_str)
		{
			query = std::move(query_str);
		}

		std::string get_query_hash() const
		{
			xxh::hash_state_t<64> hash_stream;

			nlohmann::json query_json{};
			to_json(query_json, *this);

			hash_stream.update(query_json.dump());

			return std::to_string(hash_stream.digest());
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
