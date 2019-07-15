#pragma once
#include "stdafx.h"
#include  "json_utils.h"
#include "DateTimeOffset.h"
#include "QueryTimings.h"

namespace ravendb::client::documents::queries
{
	template<typename TResult, typename TInclude>
	struct QueryResultBase
	{
		virtual ~QueryResultBase() = 0;

		TResult results;
		TInclude includes;
		nlohmann::json counter_includes = nlohmann::json::object();
		std::unordered_map<std::string, std::vector<std::string>> included_counter_names{};
		std::vector<std::string> included_paths;
		bool is_stale;
		impl::DateTimeOffset index_timestamp;
		std::string index_name;
		std::optional<int64_t> result_etag;
		impl::DateTimeOffset last_query_time;
		std::string node_tag;
		std::optional<timings::QueryTimings> timings{};
	};

	template <typename TResult, typename TInclude>
	QueryResultBase<TResult, TInclude>::~QueryResultBase() = default;

	template<typename TResult, typename TInclude>
	void from_json(const nlohmann::json& j, QueryResultBase<TResult, TInclude>& qrb)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Results", qrb.results);
		get_val_from_json(j, "Includes", qrb.includes);
		get_val_from_json(j, "CounterIncludes", qrb.counter_includes);
		get_val_from_json(j, "IncludedCounterNames", qrb.included_counter_names);
		get_val_from_json(j, "IncludedPaths", qrb.included_paths);
		get_val_from_json(j, "IsStale", qrb.is_stale);
		get_val_from_json(j, "IndexTimestamp", qrb.index_timestamp);
		get_val_from_json(j, "IndexName", qrb.index_name);
		get_val_from_json(j, "ResultEtag", qrb.result_etag);
		get_val_from_json(j, "LastQueryTime", qrb.last_query_time);
		get_val_from_json(j, "NodeTag", qrb.node_tag);
		get_val_from_json(j, "Timings", qrb.timings);
	}
}
