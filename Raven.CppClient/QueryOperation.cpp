#include "stdafx.h"
#include "QueryOperation.h"
#include "IndexDoesNotExistException.h"
#include "TimeoutException.h"

namespace ravendb::client::documents::session::operations
{
	void QueryOperation::assert_page_size_set() const
	{
		if(!_session.lock()->get_conventions()->is_throw_if_query_page_size_is_not_set())
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

	QueryOperation::QueryOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::optional<std::string> index_name,
		queries::IndexQuery index_query, std::shared_ptr<tokens::FieldsToFetchToken> fields_to_fetch,
		bool disable_entities_tracking, bool metadata_only, bool index_entries_only)
		: _session(session)
		, _index_name(std::move(index_name))
		, _index_query(std::move(index_query))
		, _metadata_only(metadata_only)
		, _index_entries_only(index_entries_only)
		, _fields_to_fetch(fields_to_fetch)
		, _no_tracking(disable_entities_tracking)
	{
		assert_page_size_set();
	}

	commands::QueryCommand QueryOperation::create_request() const
	{
		return commands::QueryCommand(_session.lock()->get_conventions(), _index_query, _metadata_only, _index_entries_only);
	}

	const queries::QueryResult& QueryOperation::get_current_query_results() const
	{
		return _current_query_result;
	}

	void QueryOperation::set_result(std::shared_ptr<const queries::QueryResult> query_result)
	{
		ensure_is_acceptable_and_save_result(query_result);
	}

	void QueryOperation::ensure_is_acceptable_and_save_result(std::shared_ptr<const queries::QueryResult> result)
	{
		if(!result)
		{
			throw exceptions::documents::indexes::IndexDoesNotExistException("Could not find index " + *_index_name);
		}
		ensure_is_acceptable(*result, _index_query.wait_for_non_stale_results, _stop_watch);

		_current_query_result = *result;

		//TODO add the logger treatment 
	}

	void QueryOperation::ensure_is_acceptable(const queries::QueryResult& result, bool wait_for_non_stale_result,
		impl::SimpleStopWatch& stop_watch)
	{
		if (wait_for_non_stale_result && result.is_stale)
		{
			auto duration = stop_watch.millis_elapsed();
			std::ostringstream msg{};
			msg << "Waited for ";
			msg << duration.count();
			msg << "milliseconds for the query to return non stale result.";

			throw exceptions::TimeoutException(msg.str());
		}
	}

	const queries::IndexQuery& QueryOperation::get_index_query() const
	{
		return _index_query;
	}
}
