#pragma once
#include "IndexQueryBase.h"
#include "Parameters.h"

namespace ravendb::client::documents::queries
{
	struct IndexQuery : IndexQueryBase<Parameters>
	{
		bool skip_duplicate_checking = false;

		bool explain_scores = false;

		bool show_timings = false;

		bool disable_caching = false;

	public:
		IndexQuery() = default;

		explicit IndexQuery(std::string query_str);

		//using https://github.com/RedSpah/xxhash_cpp
		std::string get_query_hash() const;
	};

	void to_json(nlohmann::json& j, const IndexQuery& iq);
}
