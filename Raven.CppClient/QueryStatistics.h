#pragma once
#include "DateTimeOffset.h"
#include "QueryResult.h"

namespace ravendb::client::documents::session
{
	struct QueryStatistics
	{
		bool is_stale{};
		int64_t duration_in_ms{};
		int32_t total_results{};
		int32_t skipped_results{};
		impl::DateTimeOffset timestamp{};
		std::string index_name{};
		impl::DateTimeOffset index_timestamp{};
		impl::DateTimeOffset last_query_time{};
		std::optional<int64_t> result_etag{};
		int64_t result_size{};
		std::string node_tag{};

		void update_query_stats(const queries::QueryResult& qr);
	};
}
