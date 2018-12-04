#pragma once
#include "stdafx.h"
#include  "json_utils.h"

namespace ravendb::client::documents::queries
{
	template<typename TResult, typename TInclude>
	struct QueryResultBase
	{
		virtual ~QueryResultBase() = 0;

		TResult results;
		TInclude includes;
		std::vector<std::string> included_paths;
		bool is_stale;
		std::chrono::system_clock::time_point index_timestamp;
		std::string index_name;
		int64_t result_etag;
		std::chrono::system_clock::time_point last_query_time;
		std::string node_tag;
	};

	template <typename TResult, typename TInclude>
	QueryResultBase<TResult, TInclude>::~QueryResultBase() = default;

	template<typename TResult, typename TInclude>
	inline void from_json(const nlohmann::json& j, QueryResultBase<TResult, TInclude>& qrb)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Results", qrb.results);
		get_val_from_json(j, "Includes", qrb.includes);
		get_val_from_json(j, "IncludedPaths", qrb.included_paths);
		get_val_from_json(j, "IsStale", qrb.is_stale);
		//get_val_from_json(j, "IndexTimestamp", qrb.index_timestamp);
		get_val_from_json(j, "IndexName", qrb.index_name);
		get_val_from_json(j, "ResultEtag", qrb.result_etag);
		//get_val_from_json(j, "LastQueryTime", qrb.last_query_time);
		get_val_from_json(j, "NodeTag", qrb.node_tag);
	}
}
