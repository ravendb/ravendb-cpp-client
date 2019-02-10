#include "stdafx.h"
#include "AbstractDocumentQuery.h"
#include "IndexQuery.h"
#include "DocumentSessionImpl.h"
#include "RequestExecutor.h"
#include "QueryOperation.h"

namespace ravendb::client::documents::session
{
	std::chrono::milliseconds AbstractDocumentQuery::default_timeout = std::chrono::seconds(15);

	const std::optional<std::string>& AbstractDocumentQuery::get_index_name() const
	{
		return  index_name;
	}

	const std::optional<std::string>& AbstractDocumentQuery::get_collection_name() const
	{
		return collection_name;
	}


	std::reference_wrapper<DocumentSessionImpl> AbstractDocumentQuery::get_session() const
	{
		return static_cast<DocumentSessionImpl&>(the_session.get());
	}

	queries::IndexQuery AbstractDocumentQuery::get_index_query() const
	{
		std::ostringstream query{};
		query << *this;
		auto index_query = this->generate_index_query(query.str());
		//TODO
		//invoke_before_query_executed(indexQuery);
		return index_query;
	}

	AbstractDocumentQuery::AbstractDocumentQuery(InMemoryDocumentSessionOperations& session, std::optional<std::string> index_name,
		std::optional<std::string> collection_name, bool is_group_by, /*DeclareToken*/ void* declare_token,
		/*std::vector<DeclareToken>*/void* load_tokens, std::optional<std::string> from_alias)
		: index_name(std::move(index_name))
		, collection_name(std::move(collection_name))
		, the_session(session)
	{}

	void AbstractDocumentQuery::_wait_for_non_stale_results(std::optional<std::chrono::milliseconds> wait_timeout)
	{
		the_wait_for_non_stale_results = true;
		timeout = wait_timeout ? *wait_timeout : default_timeout;
	}


	void AbstractDocumentQuery::assert_no_raw_query() const
	{
		if(query_raw)
		{
			throw std::runtime_error("RawQuery was called, cannot modify this query by calling on operations "
				"that would modify the query (such as Where, Select, OrderBy, GroupBy, etc)");
		}
	}

	operations::QueryOperation AbstractDocumentQuery::initialize_query_operation() const
	{
		auto&& index_query = get_index_query();

		return operations::QueryOperation(the_session, index_name, index_query, fields_to_fetch_token, disable_entities_tracking,
			false, false);
	}

	void AbstractDocumentQuery::_take(int32_t count)
	{
		page_size = count;
	}

	void AbstractDocumentQuery::_skip(int32_t count)
	{
		start = count;
	}



	queries::IndexQuery AbstractDocumentQuery::generate_index_query(std::string query) const
	{
		auto index_query = queries::IndexQuery(query);
		index_query.start = start;
		index_query.wait_for_non_stale_results = the_wait_for_non_stale_results;
		index_query.wait_for_non_stale_results_timeout = timeout;
		index_query.query_parameters = query_parameters;
		index_query.disable_caching = disable_caching;
		if (page_size)
		{
			index_query.page_size = page_size;
		}
		return index_query;
	}

	std::ostream& operator<<(std::ostream& os, const AbstractDocumentQuery& adq)
	{
		if(adq.query_raw)
		{
			os << *adq.query_raw;
			return os;
		}
		//TODO add later
		//if (_current_clause_depth != 0)
		//{
		//	throw std::runtime_error("A clause was not closed correctly within this query, current clause depth = " +
		//		_current_clause_depth);
		//}
		//build_declare(queryText);
		//build_from(queryText);
		//build_groupBy(queryText);
		//build_where(queryText);
		//build_order_by(queryText);

		//build_load(queryText);
		//build_select(queryText);
		//build_include(queryText);

		return os;
	}

	const std::optional<operations::QueryOperation>& AbstractDocumentQuery::get_query_operation() const
	{
		return query_operation;
	}

	void AbstractDocumentQuery::_no_tracking()
	{
		disable_entities_tracking = true;
	}

	void AbstractDocumentQuery::_no_caching()
	{
		disable_caching = true;
	}

	void AbstractDocumentQuery::init_sync()
	{
		if(query_operation)
		{
			return;
		}

		query_operation.emplace(initialize_query_operation());
		execute_actual_query();
	}

	void AbstractDocumentQuery::execute_actual_query()
	{
		auto command = query_operation->create_request();
		the_session.get().get_request_executor()->execute(command/*, the_session.get()._session_info*/);
		query_operation->set_result(command.get_result());
	}

	queries::QueryResult AbstractDocumentQuery::get_query_result()
	{
		init_sync();

		return query_operation->get_current_query_results().create_snapshot();
	}
}
