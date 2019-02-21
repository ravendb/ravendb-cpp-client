#include "stdafx.h"
#include "QueryOperation.h"

namespace ravendb::client::documents::session::operations
{
	void QueryOperation::assert_page_size_set() const
	{
		//TODO
		//if(_session.get().get_conventions().is_throw_if_query_page_size_is_not_set())
		{
			return;
		}
		if (_index_query.page_size)
		{
			return;
		}

		throw std::runtime_error("Attempt to query without explicitly specifying a page size. "
			"You can use .take() methods to set maximum number of results. "
			"By default the page size is set to std::INT32_MAX and can cause severe performance degradation.");
	}

	QueryOperation::QueryOperation(InMemoryDocumentSessionOperations& session, std::optional<std::string> index_name,
		queries::IndexQuery index_query, std::optional<tokens::FieldsToFetchToken> fields_to_fetch,
		bool disable_entities_tracking, bool metadata_only, bool index_entries_only)
		: _session(session)
		, _index_name(std::move(index_name))
		, _index_query(std::move(index_query))
		, _metadata_only(metadata_only)
		, _index_entries_only(index_entries_only)
		, _fields_to_fetch(std::move(fields_to_fetch))
		, _no_tracking(disable_entities_tracking)
	{
		assert_page_size_set();
	}

	commands::QueryCommand QueryOperation::create_request() const
	{
		return commands::QueryCommand(_session.get().get_conventions(), _index_query, _metadata_only, _index_entries_only);
	}

	const QueryResult& QueryOperation::get_current_query_results() const
	{
		return _current_query_result;
	}

	void QueryOperation::set_result(const QueryResult& query_result)
	{
		ensure_is_acceptable_and_save_result(query_result);
	}

	void QueryOperation::ensure_is_acceptable_and_save_result(const QueryResult& result)
	{
		ensure_is_acceptable(result, _index_query.wait_for_non_stale_results, _start_time_for_stopwatch);

		_current_query_result = result;

		//TODO add the logger treatment 
	}

	void QueryOperation::ensure_is_acceptable(const QueryResult& result, bool wait_for_non_stale_result,
		const std::chrono::time_point<std::chrono::steady_clock>& start_time_for_stopwatch)
	{
		if (wait_for_non_stale_result && result.is_stale)
		{
			const auto stop_time_for_stopwatch = std::chrono::steady_clock::now();

			std::ostringstream msg{};
			msg << "Waited for ";
			msg << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_for_stopwatch - start_time_for_stopwatch).count();
			msg << "milliseconds for the query to return non stale result.";

			throw std::out_of_range(msg.str());
		}
	}

	const IndexQuery& QueryOperation::get_index_query() const
	{
		return _index_query;
	}
}