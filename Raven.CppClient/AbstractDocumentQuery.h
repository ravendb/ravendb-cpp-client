#pragma once
#include <unordered_set>
#include <ostream>
#include "QueryOperator.h"
#include "IndexQuery.h"
#include "QueryResult.h"
#include "QueryOperation.h"
#include "Lazy.h"
#include "RequestExecutor.h"
#include "LazyQueryOperation.h"
#include "DeclareToken.h"
#include "FromToken.h"
#include "OrderingType.h"

namespace ravendb::client::documents::session
{
	class DocumentSessionImpl;
}

namespace ravendb::client::documents::session
{
	template<typename T>
	class AbstractDocumentQuery
	{
	private:
		static std::chrono::milliseconds default_timeout;

		std::optional<std::string> index_name{};

		std::optional<std::string> collection_name {};

		int32_t _current_clause_depth{};

		std::shared_ptr<DocumentConventions> _conventions;

	protected:
		queries::QueryOperator default_operator = queries::QueryOperator::AND;

		Parameters query_parameters{};

		std::shared_ptr<InMemoryDocumentSessionOperations> the_session;

		std::optional<int32_t> page_size{};

		bool is_group_by{};

		std::optional<tokens::DeclareToken> declare_token{};

		tokens::FromToken from_token;

		int32_t start{};

		std::optional<tokens::FieldsToFetchToken> fields_to_fetch_token{};

		std::chrono::milliseconds timeout = default_timeout;

		bool the_wait_for_non_stale_results{};

		std::unordered_set<std::string> document_includes{};
		
		bool disable_entities_tracking{};

		bool disable_caching{};

		std::shared_ptr<operations::QueryOperation> query_operation{};
		
		std::optional<std::string> query_raw{};

		std::vector<std::function<void(const QueryResult&)>> after_query_executed_callback{};

	private:
		void assert_no_raw_query() const;

		void execute_actual_query();

		std::vector<std::shared_ptr<T>> execute_query_operation(std::optional<int32_t> take);

		void build_declare(std::ostringstream& writer) const;

		void build_from(std::ostringstream& writer) const;

	protected:
		void _using_default_operator(queries::QueryOperator query_operator);

		IndexQuery get_index_query() const;

		void _add_parameter(std::string name, nlohmann::json object);

		void _take(int32_t count);

		void _skip(int32_t count);

		void _wait_for_non_stale_results(std::optional<std::chrono::milliseconds> wait_timeout);

		void _no_tracking();

		void _no_caching();

		void _order_by(const std::string& field, OrderingType ordering = OrderingType::STRING);

		//TODO see what to send by value/reference
		AbstractDocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::optional<std::string> index_name,
			std::optional<std::string> collection_name, bool is_group_by, std::optional<tokens::DeclareToken> declare_token,
			/*std::vector<LoadToken>*/void* load_tokens, std::optional<std::string> from_alias);

		std::shared_ptr<operations::QueryOperation> initialize_query_operation() const;

		queries::IndexQuery generate_index_query(std::string query) const;

		void init_sync();

	public:
		const std::optional<std::string>& get_index_name() const;

		const std::optional<std::string>& get_collection_name() const;

		std::shared_ptr<DocumentConventions> get_conventions() const;

		std::shared_ptr<DocumentSessionImpl> get_session() const;

		const std::shared_ptr<operations::QueryOperation>& get_query_operation() const;

		std::vector<std::shared_ptr<T>> to_list();

		int32_t count();

		queries::QueryResult get_query_result();

		Lazy<std::vector<std::shared_ptr<T>>> lazily(
			const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval);

		operator std::string() const
		{
			if (query_raw)
			{
				return *query_raw;
			}
			
			if (_current_clause_depth != 0)
			{
				throw std::runtime_error("A clause was not closed correctly within this query, current clause depth = " +
					std::to_string(_current_clause_depth));
			}

			std::ostringstream query_text{};

			build_declare(query_text);
			build_from(query_text);
			//build_groupBy(queryText);
			//build_where(queryText);
			//build_order_by(queryText);

			//build_load(queryText);
			//build_select(queryText);
			//build_include(queryText);

			return query_text.str();
		}
	};

#include "DocumentSessionImpl.h"

	template<typename T>
	std::chrono::milliseconds AbstractDocumentQuery<T>::default_timeout = std::chrono::seconds(15);

	template <typename T>
	void AbstractDocumentQuery<T>::assert_no_raw_query() const
	{
		if (query_raw)
		{
			throw std::runtime_error("RawQuery was called, cannot modify this query by calling on operations "
				"that would modify the query (such as Where, Select, OrderBy, GroupBy, etc)");
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::execute_actual_query()
	{
		auto command = query_operation->create_request();
		the_session->get_request_executor()->execute(command/*, the_session->_session_info*/);
		query_operation->set_result(*command.get_result());
	}

	template <typename T>
	std::vector<std::shared_ptr<T>> AbstractDocumentQuery<T>::execute_query_operation(std::optional<int32_t> take)
	{
		if (take && (page_size || *page_size > *take))
		{
			_take(*take);
		}
		init_sync();

		return query_operation->complete<T>();
	}

	template <typename T>
	void AbstractDocumentQuery<T>::build_declare(std::ostringstream& writer) const
	{
		if(declare_token)
		{
			declare_token->write_to(writer);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::build_from(std::ostringstream& writer) const
	{
		from_token.write_to(writer);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_using_default_operator(queries::QueryOperator query_operator)
	{
		//TODO add
		//if (!where_tokens.empty()) {
		//	throw new std::runtime_error("Default operator can only be set before any where clause is added.");
		//}

		default_operator = query_operator;
	}

	template <typename T>
	IndexQuery AbstractDocumentQuery<T>::get_index_query() const
	{
		auto index_query = this->generate_index_query(*this);
		//TODO
		//invoke_before_query_executed(indexQuery);
		return index_query;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_add_parameter(std::string name, nlohmann::json object)
	{
		const std::string pure_name(name.front() == '$' ? ++name.begin() : name.begin(), name.end());
		if (query_parameters.find(pure_name) != query_parameters.end())
		{
			std::ostringstream msg{};
			msg << "The parameter " << pure_name << " was already added";
			throw std::runtime_error(msg.str());
		}

		query_parameters.insert_or_assign(pure_name, object);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_take(int32_t count)
	{
		page_size = count;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_skip(int32_t count)
	{
		start = count;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_wait_for_non_stale_results(std::optional<std::chrono::milliseconds> wait_timeout)
	{
		the_wait_for_non_stale_results = true;
		timeout = wait_timeout ? *wait_timeout : default_timeout;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_no_tracking()
	{
		disable_entities_tracking = true;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_no_caching()
	{
		disable_caching = true;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by(const std::string& field, OrderingType ordering)
	{
		//TODO implement
		throw std::runtime_error("not implemented");
		//assert_no_raw_query();
		//auto f = ensure_valid_field_name(field, false);
		//	order_by_tokens.push_back(OrderByToken::create_ascending(f, ordering));
	}

	template <typename T>
	AbstractDocumentQuery<T>::AbstractDocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::optional<std::string> index_name_param,
		std::optional<std::string> collection_name_param, bool is_group_by_param, std::optional<tokens::DeclareToken> declare_token_param,
		/*std::vector<LoadToken>*/void* load_tokens_param, std::optional<std::string> from_alias_param)
		: index_name(std::move(index_name_param))
		, collection_name(std::move(collection_name_param))
		, _conventions(session->get_conventions())
		, the_session(session)
		, is_group_by(is_group_by_param)
		, from_token(index_name, collection_name, from_alias_param)
	{}

	template <typename T>
	std::shared_ptr<operations::QueryOperation> AbstractDocumentQuery<T>::initialize_query_operation() const
	{
		auto&& index_query = get_index_query();

		return std::make_shared<operations::QueryOperation>(the_session, index_name, index_query, fields_to_fetch_token, disable_entities_tracking,
			false, false);
	}

	template <typename T>
	queries::IndexQuery AbstractDocumentQuery<T>::generate_index_query(std::string query) const
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

	template <typename T>
	void AbstractDocumentQuery<T>::init_sync()
	{
		if (query_operation)
		{
			return;
		}

		query_operation = initialize_query_operation();
		execute_actual_query();
	}

	template <typename T>
	const std::optional<std::string>& AbstractDocumentQuery<T>::get_index_name() const
	{
		return  index_name;
	}

	template <typename T>
	const std::optional<std::string>& AbstractDocumentQuery<T>::get_collection_name() const
	{
		return collection_name;
	}

	template <typename T>
	std::shared_ptr<DocumentConventions> AbstractDocumentQuery<T>::get_conventions() const
	{
		return _conventions;
	}

	template <typename T>
	std::shared_ptr<DocumentSessionImpl> AbstractDocumentQuery<T>::get_session() const
	{

		return std::reinterpret_pointer_cast<DocumentSessionImpl>(the_session);
	}

	template <typename T>
	const std::shared_ptr<operations::QueryOperation>& AbstractDocumentQuery<T>::get_query_operation() const
	{
		return query_operation;
	}

	template <typename T>
	std::vector<std::shared_ptr<T>> AbstractDocumentQuery<T>::to_list()
	{
		return execute_query_operation({});
	}

	template <typename T>
	int32_t AbstractDocumentQuery<T>::count()
	{
		_take(0);
		return get_query_result().total_results;
	}

	template <typename T>
	queries::QueryResult AbstractDocumentQuery<T>::get_query_result()
	{
		init_sync();

		return query_operation->get_current_query_results().create_snapshot();
	}

	template <typename T>
	Lazy<std::vector<std::shared_ptr<T>>> AbstractDocumentQuery<T>::lazily(
		const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval)
	{
		if (!get_query_operation())
		{
			query_operation = initialize_query_operation();
		}

		auto lazy_query_operation = std::make_shared<operations::lazy::LazyQueryOperation<T>>(
			the_session->get_conventions(),
			query_operation,
			after_query_executed_callback);

		auto get_operation_result = [=](std::shared_ptr<operations::lazy::ILazyOperation> operation)
		{
			auto lazy_query_op = std::static_pointer_cast<operations::lazy::LazyQueryOperation<T>>(operation);
			return lazy_query_op->get_result();
		};

		return get_session()->add_lazy_operation<std::vector<std::shared_ptr<T>>>(lazy_query_operation,
			get_operation_result, [=]()
		{
			if (on_eval)
				(*on_eval)(get_operation_result(std::static_pointer_cast<operations::lazy::ILazyOperation>(lazy_query_operation)));
		});
	}
}
