#include "stdafx.h"
#include "QueryStatistics.h"

namespace ravendb::client::documents::session
{
	void QueryStatistics::update_query_stats(const queries::QueryResult& qr)
	{
		is_stale = qr.is_stale;
		duration_in_ms = qr.duration_in_ms;
		total_results = qr.total_results;
		skipped_results = qr.skipped_results;
		timestamp = qr.index_timestamp;
		index_name = qr.index_name;
		index_timestamp = qr.index_timestamp;
		last_query_time = qr.last_query_time;
		result_etag = qr.result_etag;
		node_tag = qr.node_tag;
	}
}
