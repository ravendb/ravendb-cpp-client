#pragma once
#include <unordered_set>
#include <ostream>
#include <type_traits>
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
#include "QueryStatistics.h"
#include "DistinctToken.h"
#include "LoadToken.h"
#include "OrderByToken.h"
#include "utils.h"
#include "GroupBy.h"
#include "GroupByToken.h"
#include "GroupByKeyToken.h"
#include "GroupBySumToken.h"
#include "GroupByCountToken.h"
#include "TrueToken.h"
#include "MoreLikeThisScope.h"
#include "QueryFieldUtil.h"
#include "OpenSubclauseToken.h"
#include "NegateToken.h"
#include "CloseSubclauseToken.h"
#include "QueryOperatorToken.h"
#include "WhereParams.h"
#include "MethodCall.h"
#include "CmpXchg.h"
#include "HighlightingToken.h"
#include "QueryHighlightings.h"
#include "ExplanationToken.h"
#include "Explanations.h"
#include "TimingsToken.h"
#include "IntersectMarkerToken.h"
#include "DocumentQueryHelper.h"
#include "QueryData.h"
#include "HighlightingOptions.h"
#include "SpatialRelation.h"
#include "DynamicSpatialField.h"
#include "SpatialCriteria.h"
#include "SuggestionBase.h"
#include "SuggestToken.h"
#include "SuggestionWithTerm.h"
#include "SuggestionWithTerms.h"
#include "GetCppClassName.h"
#include "ExplanationOptions.h"
#include "EventHelper.h"
#include "IncludeBuilderBase.h"

namespace ravendb::client::documents::session
{
	class DocumentSessionImpl;
}

namespace ravendb::client::documents::session
{
	template<typename T>
	class AbstractDocumentQuery
	{
	public:
		std::shared_ptr<tokens::FieldsToFetchToken> fields_to_fetch_token{};

	private:
		std::unordered_map<std::string, std::string> _alias_to_group_by_field_name{};

		static std::chrono::milliseconds DEFAULT_TIMEOUT;

		std::optional<std::string> index_name{};

		std::optional<std::string> collection_name{};

		int32_t _current_clause_depth{};

		std::shared_ptr<conventions::DocumentConventions> _conventions;

		bool _is_more_like_this{};

		std::optional<std::string> _included_alias{};

	protected:
		std::weak_ptr<AbstractDocumentQuery> _weak_this;

		queries::QueryOperator default_operator = queries::QueryOperator::AND;

		std::unordered_set<std::type_index> root_types{};

		//Whether to negate the next operation 
		bool negate{};

		std::optional<std::string> query_raw{};

		Parameters query_parameters{};

		bool is_intersect{};

		bool is_group_by{};

		std::weak_ptr<InMemoryDocumentSessionOperations> the_session;

		std::optional<int32_t> page_size{};

		std::list<std::shared_ptr<tokens::QueryToken>> select_tokens{};

		std::shared_ptr<tokens::FromToken> from_token;

		std::shared_ptr<tokens::DeclareToken> declare_token{};

		std::vector<std::shared_ptr<tokens::LoadToken>> load_tokens{};

		std::list<std::shared_ptr<tokens::QueryToken>> where_tokens{};

		std::list<std::shared_ptr<tokens::QueryToken>> group_by_tokens{};

		std::list<std::shared_ptr<tokens::QueryToken>> order_by_tokens{};

		int32_t start{};

		std::chrono::milliseconds timeout = DEFAULT_TIMEOUT;

		bool the_wait_for_non_stale_results{};

		std::unordered_set<std::string> document_includes{};

		//Holds the query stats
		std::shared_ptr<QueryStatistics> query_stats = std::make_shared<QueryStatistics>();

		bool disable_entities_tracking{};

		bool disable_caching{};

		std::vector<std::function<void(const queries::IndexQuery&)>> before_query_executed_callback{};

		std::vector<std::function<void(const queries::QueryResult&)>> after_query_executed_callback{};

		std::vector<std::function<void(const nlohmann::json&)>> after_stream_executed_callback{};

		std::shared_ptr<operations::QueryOperation> query_operation{};

		std::shared_ptr<queries::timings::QueryTimings> query_timings{};

		std::vector<std::shared_ptr<tokens::HighlightingToken>> highlighting_tokens{};

		queries::highlighting::QueryHighlightings query_highlightings{};

		std::shared_ptr<queries::explanation::Explanations> explanations{};

		std::shared_ptr<tokens::ExplanationToken> explanation_token{};

		//TODO
		//protected List<CounterIncludesToken> counterIncludesTokens;

	private:
		static std::chrono::milliseconds get_default_timeout();

		void assert_no_raw_query() const;

		template<typename TValue>
		bool if_value_is_method(tokens::WhereOperator op, const WhereParams<TValue>& where_params,
			std::list<std::shared_ptr<tokens::QueryToken>>& tokens);

		void build_include(std::ostringstream& writer) const;

		void update_stats_highlightings_and_explanations(const queries::QueryResult& query_result);

		void build_select(std::ostringstream& writer) const;

		void build_from(std::ostringstream& writer) const;

		void build_declare(std::ostringstream& writer) const;

		void build_load(std::ostringstream& writer) const;

		void build_where(std::ostringstream& writer) const;

		void build_group_by(std::ostringstream& writer) const;

		void build_order_by(std::ostringstream& writer) const;

		void append_operator_if_needed(std::list<std::shared_ptr<tokens::QueryToken>>& tokens);

		template<typename TValue>
		nlohmann::json transform_collection(const std::string& field_name,
			const std::vector<TValue>& values) const;

		void negate_if_needed(std::list<std::shared_ptr<tokens::QueryToken>>& tokens, const std::optional<std::string>& field_name);

		std::string ensure_valid_field_name(const std::string& field_name, bool is_nested_path) const;

		template<typename TValue>
		nlohmann::json transform_value(const WhereParams<TValue>& where_params, bool for_range = false) const;

		std::string add_query_parameter(nlohmann::json value);

		std::list<std::shared_ptr<tokens::QueryToken>>& get_current_where_tokens();

		void assert_is_dynamic_query(const queries::spatial::DynamicSpatialField& dynamic_field,
			const std::string& method_name);

		void execute_actual_query();

		std::vector<std::shared_ptr<T>> execute_query_operation(std::optional<int32_t> take,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json);

		void assert_can_suggest() const;

	protected:
		AbstractDocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session,
			std::optional<std::string> index_name_param,
			std::optional<std::string> collection_name_param,
			bool is_group_by_param,
			std::shared_ptr<tokens::DeclareToken> declare_token_param,
			std::vector<std::shared_ptr<tokens::LoadToken>> load_tokens_param,
			std::optional<std::string> from_alias = {});

		//call AFTER! the _weak_ptr is set.
		void complete_construction();

		std::shared_ptr<operations::QueryOperation> initialize_query_operation();

		queries::IndexQuery get_index_query();

		void add_group_by_alias(std::string field_name, std::string projected_name);

		queries::IndexQuery generate_index_query(std::string query) const;

		void update_fields_to_fetch_token(std::shared_ptr<tokens::FieldsToFetchToken> fields_to_fetch);

		template<typename Type>
		static void get_source_alias_if_exists(const queries::QueryData& query_data,
			const std::vector<std::string>& fields, std::optional<std::string>& source_alias_reference);

		void _include_timings(std::shared_ptr<queries::timings::QueryTimings>& timings);

		void _within_radius_of(const std::string& field_name, double radius, double latitude, double longitude,
			const std::optional<indexes::spatial::SpatialUnits>& radius_units, double dist_error_percent);

		void _spatial(const std::string& field_name, const std::string& shape_wkt, indexes::spatial::SpatialRelation relation,
			const std::optional<indexes::spatial::SpatialUnits>& units, double dist_error_percent);

		void init_sync();

		//TODO
		//protected void _includeCounters(String alias, Map<String, Tuple<Boolean, Set<String>>> counterToIncludeByDocId) {

	public:
		const std::optional<std::string>& get_index_name() const;

		const std::optional<std::string>& get_collection_name() const;

		bool is_distinct() const;

		std::shared_ptr<conventions::DocumentConventions> get_conventions() const;

		std::shared_ptr<DocumentSessionImpl> get_session() const;

		bool is_dynamic_map_reduce() const;

		void _using_default_operator(queries::QueryOperator query_operator);

		void _wait_for_non_stale_results(const std::optional<std::chrono::milliseconds>& wait_timeout);

		std::vector<std::string> get_projection_fields() const;

		void _random_ordering();
		void _random_ordering(const std::string& seed);

		//TODO (from JAVA)
		//TBD 4.1 public void _customSortUsing(String typeName)
		//TBD 4.1 public void _customSortUsing(String typeName, boolean descending)

		void _add_parameter(std::string name, nlohmann::json object);

		void _group_by(const std::vector<std::string>& field_names);
		void _group_by(const std::vector<queries::GroupBy>& fields);

		void _group_by_key(const std::optional<std::string>& field_name, const std::optional<std::string>& projected_name = {});

		void _group_by_sum(const std::string& field_name, const std::optional<std::string>& projected_name = {});

		void _group_by_count(const std::optional<std::string>& projected_name = {});

		void _where_true();

		queries::more_like_this::MoreLikeThisScope _more_like_this();

		void _include(std::string path);

		void _include(std::shared_ptr<loaders::IncludeBuilderBase> includes);

		void _take(int32_t count);

		void _skip(int32_t count);

		void _where_lucene(const std::string& field_name, const std::string& where_clause, bool exact);

		void _open_subclause();

		void _close_subclause();

		template<typename TValue>
		void _where_equals(std::string field_name, const TValue* value, bool exact = false);

		void _where_equals(std::string field_name, const MethodCall* method, bool exact = false);

		template<typename TValue>
		void _where_equals(WhereParams<TValue> where_params);

		template<typename TValue>
		void _where_not_equals(std::string field_name, const TValue* value, bool exact = false);

		void _where_not_equals(std::string field_name, const MethodCall* method, bool exact = false);

		template<typename TValue>
		void _where_not_equals(WhereParams<TValue> where_params);

		void negate_next();

		template<typename TValue>
		void _where_in(const std::string& field_name, const std::vector<TValue>& values, bool exact = false);

		template<typename TValue>
		void _where_starts_with(std::string field_name, const TValue* value);

		template<typename TValue>
		void _where_ends_with(std::string field_name, const TValue* value);

		template<typename TValue>
		void _where_between(const std::string& field_name, const TValue* start, const TValue* end, bool exact = false);

		template<typename TValue>
		void _where_greater_than(const std::string& field_name, const TValue* value, bool exact = false);

		template<typename TValue>
		void _where_greater_than_or_equal(const std::string& field_name, const TValue* value, bool exact = false);

		template<typename TValue>
		void _where_less_than(const std::string& field_name, const TValue* value, bool exact = false);

		template<typename TValue>
		void _where_less_than_or_equal(const std::string& field_name, const TValue* value, bool exact = false);

		void _where_regex(const std::string& field_name, std::string pattern);

		void _and_also();

		void _or_else();

		void _boost(double boost);

		void _fuzzy(double fuzzy);

		void _proximity(int32_t proximity);

		void _order_by(const std::string& field, OrderingType ordering = OrderingType::STRING);

		void _order_by_descending(const std::string& field, OrderingType ordering = OrderingType::STRING);

		void _order_by_score();

		void _order_by_score_descending();

		void _statistics(std::shared_ptr<QueryStatistics>& stats) const;

		void invoke_after_query_executed(const queries::QueryResult& result);

		void invoke_before_query_executed(const queries::IndexQuery& query);

		void invoke_after_stream_executed(const nlohmann::json& result);

		void _search(const std::string& field_name, std::string search_terms,
			queries::SearchOperator search_operator = queries::SearchOperator::OR);

		operator std::string() const;

		void _intersect();

		void _where_exists(const std::string& field_name);

		template<typename TValue>
		void _contains_any(const std::string& field_name, const std::vector<TValue>& values);

		template<typename TValue>
		void _contains_all(const std::string& field_name, const std::vector<TValue>& values);

		void _add_root_type(std::type_index type);

		void _distinct();

		void add_from_alias_to_where_token(const std::string& from_alias);

		//TODO
		// public String addAliasToCounterIncludesTokens(String fromAlias) 

		std::shared_ptr<operations::QueryOperation> get_query_operation() const;

		void _add_before_query_executed_listener(std::function<void(const queries::IndexQuery&)> action);
		void _remove_before_query_executed_listener(std::function<void(const queries::IndexQuery&)> action);


		void _add_after_query_executed_listener(std::function<void(const queries::QueryResult&)> action);
		void _remove_after_query_executed_listener(std::function<void(const queries::QueryResult&)> action);


		void _add_after_stream_executed_listener(std::function<void(const nlohmann::json&)> action);
		void _remove_after_stream_executed_listener(std::function<void(const nlohmann::json&)> action);

		void _no_tracking();

		void _no_caching();

		void _highlight(std::string field_name, int32_t fragment_length, int32_t fragment_count,
			const std::optional<queries::highlighting::HighlightingOptions>& options,
			std::shared_ptr<queries::highlighting::Highlightings>& highlightings_reference);

		void _spatial(const std::string& field_name, const queries::spatial::SpatialCriteria& criteria);
		void _spatial(const queries::spatial::DynamicSpatialField& dynamic_field, const queries::spatial::SpatialCriteria& criteria);

		void _order_by_distance(const queries::spatial::DynamicSpatialField& field, double latitude, double longitude);
		void _order_by_distance(const std::string& field_name, double latitude, double longitude);
		void _order_by_distance(const queries::spatial::DynamicSpatialField& field, const std::string& shape_wkt);
		void _order_by_distance(const std::string& field_name, const std::string& shape_wkt);

		void _order_by_distance_descending(const queries::spatial::DynamicSpatialField& field, double latitude, double longitude);
		void _order_by_distance_descending(const std::string& field_name, double latitude, double longitude);
		void _order_by_distance_descending(const queries::spatial::DynamicSpatialField& field, const std::string& shape_wkt);
		void _order_by_distance_descending(const std::string& field_name, const std::string& shape_wkt);

		std::vector<std::shared_ptr<T>> to_list(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		queries::QueryResult get_query_result();

		std::shared_ptr<T> first(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});
		std::shared_ptr<T> first_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		std::shared_ptr<T> single(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});
		std::shared_ptr<T> single_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		int32_t count();

		bool any();

		//TODO
		//public void _aggregateBy(FacetBase facet)
		//public void _aggregateUsing(String facetSetupDocumentId) 

		Lazy<std::vector<std::shared_ptr<T>>> lazily(
			const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval = {});

		Lazy<int32_t> count_lazily();

		void _suggest_using(const queries::suggestions::SuggestionBase& suggestion);

		std::optional<std::string> get_options_parameter_name(const std::optional<queries::suggestions::SuggestionOptions>& options);

		void _include_explanations(const std::optional<queries::explanation::ExplanationOptions>& options,
			std::shared_ptr<queries::explanation::Explanations>& explanations_reference);
	};
}

#include "DocumentSessionImpl.h"

namespace ravendb::client::documents::session
{
	template<typename T>
	std::chrono::milliseconds AbstractDocumentQuery<T>::DEFAULT_TIMEOUT = std::chrono::seconds(15);

	template <typename T>
	std::chrono::milliseconds AbstractDocumentQuery<T>::get_default_timeout()
	{
		return DEFAULT_TIMEOUT;
	}

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
		{
			auto command = query_operation->create_request();
			the_session.lock()->get_request_executor()->execute(command, the_session.lock()->get_session_info());
			query_operation->set_result(command.get_result());
		}

		invoke_after_query_executed(query_operation->get_current_query_results());
	}

	template <typename T>
	std::shared_ptr<T> AbstractDocumentQuery<T>::first(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		auto&& results = execute_query_operation(1, from_json);
		if(results.empty())
		{
			throw std::runtime_error("Expected at least one result");
		}

		return  results[0];
	}

	template <typename T>
	std::shared_ptr<T> AbstractDocumentQuery<T>::first_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		auto&& results = execute_query_operation(1, from_json);
		
		if (results.empty() || !results[0])
		{
			if constexpr (std::is_default_constructible_v<T>)
			{
				return std::make_shared<T>();
			}
			else
			{
				return std::shared_ptr<T>();
			}
		}
		else
		{
			return results[0];
		}
	}

	template <typename T>
	std::shared_ptr<T> AbstractDocumentQuery<T>::single(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		auto&& results = execute_query_operation(2, from_json);
		if(results.size() != 1)
		{
			throw std::runtime_error("Expected single result, got: " + std::to_string(results.size()));
		}

		return results[0];
	}

	template <typename T>
	std::shared_ptr<T> AbstractDocumentQuery<T>::single_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		auto&& results = execute_query_operation(2, from_json);
		if (results.size() > 1)
		{
			throw std::runtime_error("Expected single result, got: " + results.size());
		}

		return results.empty() || !results[0] ? std::make_shared<T>() : results[0];

	}

	template <typename T>
	std::vector<std::shared_ptr<T>> AbstractDocumentQuery<T>::execute_query_operation(std::optional<int32_t> take,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		if (take && (page_size || *page_size > *take))
		{
			_take(*take);
		}
		init_sync();

		return query_operation->complete<T>(from_json);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::assert_can_suggest() const
	{
		if(!where_tokens.empty())
		{
			throw std::runtime_error("Cannot add suggest when WHERE statements are present.");
		}

		if (!select_tokens.empty())
		{
			throw std::runtime_error("Cannot add suggest when SELECT statements are present.");
		}

		if (!order_by_tokens.empty())
		{
			throw std::runtime_error("Cannot add suggest when ORDER BY statements are present.");
		}
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
	void AbstractDocumentQuery<T>::build_load(std::ostringstream& writer) const
	{
		if(load_tokens.empty())
		{
			return;
		}

		writer << " load ";

		for(auto it = load_tokens.begin(); it != load_tokens.end(); ++it)
		{
			if(it != load_tokens.begin())
			{
				writer << ", ";
			}
			(*it)->write_to(writer);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::build_where(std::ostringstream& writer) const
	{
		if(where_tokens.empty())
		{
			return;
		}

		writer << " where ";
		if(is_intersect)
		{
			writer << "intersect(";
		}

		for(auto it = where_tokens.begin(); it != where_tokens.end(); ++it)
		{
			DocumentQueryHelper::add_space_if_needed(it != where_tokens.begin() ? *std::prev(it) : std::shared_ptr<tokens::QueryToken>(),
				*it, writer);
			(*it)->write_to(writer);
		}
		if(is_intersect)
		{
			writer << ") ";
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::build_group_by(std::ostringstream& writer) const
	{
		if(group_by_tokens.empty())
		{
			return;
		}

		writer << " group by ";

		for(auto it = group_by_tokens.begin(); it != group_by_tokens.end(); ++it)
		{
			if(it != group_by_tokens.begin())
			{
				writer << ", ";
			}
			(*it)->write_to(writer);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::build_order_by(std::ostringstream& writer) const
	{
		if (order_by_tokens.empty())
		{
			return;
		}

		writer << " order by ";

		for (auto it = order_by_tokens.begin(); it != order_by_tokens.end(); ++it)
		{
			if (it != order_by_tokens.begin())
			{
				writer << ", ";
			}
			(*it)->write_to(writer);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::build_from(std::ostringstream& writer) const
	{
		from_token->write_to(writer);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::append_operator_if_needed(std::list<std::shared_ptr<tokens::QueryToken>>& tokens)
	{
		assert_no_raw_query();

		if(tokens.empty())
		{
			return;
		}

		if (auto& last_token = tokens.back();
			!std::dynamic_pointer_cast<tokens::WhereToken>(last_token) && 
			!std::dynamic_pointer_cast<tokens::CloseSubclauseToken>(last_token))
		{
			return;
		}

		std::shared_ptr<tokens::WhereToken> last_where{};

		for(auto it = tokens.rbegin(); it != tokens.rend(); ++it)
		{
			if(last_where =/*assignment*/ std::dynamic_pointer_cast<tokens::WhereToken>(*it))
			{
				break;
			}
		}

		auto token = (default_operator == queries::QueryOperator::AND) ? tokens::QueryOperatorToken::AND :
			tokens::QueryOperatorToken::OR;

		if(last_where && last_where->options.search_operator)
		{
			token = tokens::QueryOperatorToken::OR;// default to OR operator after search if AND was not specified explicitly
		}

		tokens.push_back(token);
	}

	template <typename T>
	template <typename TValue>
	nlohmann::json AbstractDocumentQuery<T>::transform_collection(const std::string& field_name,
		const std::vector<TValue>& values) const
	{
		nlohmann::json result = nlohmann::json::array();

		for(const auto& value : values)
		{
			auto nested_where_params = WhereParams<TValue>();
			nested_where_params.allow_wildcards = true;
			nested_where_params.field_name = field_name;
			nested_where_params.value = &value;

			result.push_back(transform_value(nested_where_params));
		}
		return std::move(result);
	}

	template <typename T>
	std::string AbstractDocumentQuery<T>::ensure_valid_field_name(const std::string& field_name,
		bool is_nested_path) const
	{
		auto session = the_session.lock();
		if(!session || !session->get_conventions() || is_nested_path || is_group_by)
		{
			return queries::QueryFieldUtil::escape_if_necessary(field_name);
		}

		for(auto& root_type : root_types)
		{
			auto id_helper = session->get_conventions()->get_entity_id_helper(root_type);
			if(id_helper && id_helper->get_id_field_name() == field_name)
			{
				return constants::documents::indexing::fields::DOCUMENT_ID_FIELD_NAME;
			}
		}

		return queries::QueryFieldUtil::escape_if_necessary(field_name);
	}

	template <typename T>
	template <typename TValue>
	nlohmann::json AbstractDocumentQuery<T>::transform_value(const WhereParams<TValue>& where_params, bool for_range) const
	{
		if constexpr (std::is_null_pointer_v<TValue>)
		{
			return nullptr;
		}

		if(!where_params.value)
		{
			return nullptr;
		}

		if constexpr (std::is_same_v<TValue, std::string>)
		{
			return *where_params.value;
		}

		if constexpr (std::is_array_v<TValue>)
		{
			if constexpr (std::is_same_v<char, std::remove_all_extents_t<TValue>>)
			{
				return std::string(*where_params.value);
			}
		}

		if constexpr (std::is_same_v<std::remove_cv_t<TValue>, char>)
		{
			return std::string(1, *where_params.value);
		}

		if constexpr (std::is_fundamental_v<std::remove_cv_t<TValue>>)
		{
			return *where_params.value;
		}

		auto res = _conventions->try_convert_value_for_query(where_params.field_name, *where_params.value, for_range);
		if(res)
		{
			return *std::move(res);
		}

		return *where_params.value;
	}

	template <typename T>
	std::string AbstractDocumentQuery<T>::add_query_parameter(nlohmann::json value)
	{
		auto parameter_name = "p" + std::to_string(query_parameters.size());
		query_parameters.insert_or_assign(parameter_name, std::move(value));
		return std::move(parameter_name);
	}

	template <typename T>
	std::list<std::shared_ptr<tokens::QueryToken>>& AbstractDocumentQuery<T>::get_current_where_tokens()
	{
		if(!_is_more_like_this)
		{
			return where_tokens;
		}

		if(where_tokens.empty())
		{
			throw std::runtime_error("Cannot get MoreLikeThisToken because there are no where token specified.");
		}

		auto& last_token = where_tokens.back();

		if(auto more_like_this_token = std::dynamic_pointer_cast<tokens::MoreLikeThisToken>(last_token);
			more_like_this_token)
		{
			return more_like_this_token->where_tokens;
		}
		else
		{
			throw std::runtime_error("Last token is not MoreLikeThisToken");
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::assert_is_dynamic_query(const queries::spatial::DynamicSpatialField& dynamic_field,
		const std::string& method_name)
	{
		if(!from_token->is_dynamic())
		{
			std::ostringstream msg{};
			msg << "Cannot execute query method '" << method_name << "'. Field '" 
				<<  dynamic_field.to_field([this](const std::string& field_name, bool is_nested_path)
			{
				return ensure_valid_field_name(field_name, is_nested_path);
			})
				<< "' cannot be used when static index '" << from_token->get_index_name().value_or("")
				<< "' is queried. Dynamic spatial fields can only be used with dynamic queries, "
				<< "for static index queries please use valid spatial fields defined in index definition.";

			throw std::runtime_error(msg.str());
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_using_default_operator(queries::QueryOperator query_operator)
	{
		if (!where_tokens.empty())
		{
			throw new std::runtime_error("Default operator can only be set before any where clause is added.");
		}

		default_operator = query_operator;
	}

	template <typename T>
	queries::IndexQuery AbstractDocumentQuery<T>::get_index_query()
	{
		auto index_query = this->generate_index_query(*this);

		invoke_before_query_executed(index_query);
		return index_query;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::add_group_by_alias(std::string field_name, std::string projected_name)
	{
		_alias_to_group_by_field_name.insert_or_assign(std::move(projected_name), std::move(field_name));
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
	void AbstractDocumentQuery<T>::_group_by(const std::vector<std::string>& field_names)
	{
		auto mapping = std::vector<queries::GroupBy>{};
		mapping.reserve(field_names.size());
		std::transform(field_names.begin(), field_names.end(), std::back_inserter(mapping),
			[](const std::string& field_name)
		{
			return queries::GroupBy::field(field_name);
		});

		_group_by(mapping);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_group_by(const std::vector<queries::GroupBy>& fields)
	{
		if(!from_token->is_dynamic())
		{
			throw std::runtime_error("group_by only works with dynamic queries");
		}

		assert_no_raw_query();
		is_group_by = true;

		for (const auto& item : fields)
		{
			auto field_name = ensure_valid_field_name(item.get_field(), false);
			group_by_tokens.push_back(tokens::GroupByToken::create(field_name, item.get_method()));
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_group_by_key(const std::optional<std::string>& field_name,
		const std::optional<std::string>& projected_name)
	{
		assert_no_raw_query();
		is_group_by = true;
		std::optional<std::string> final_field_name = field_name;

		if(projected_name && _alias_to_group_by_field_name.find(*projected_name) != _alias_to_group_by_field_name.end())
		{
			auto& aliased_filed_name = _alias_to_group_by_field_name.at(*projected_name);
			if(!field_name || impl::utils::CompareStringsLessThanIgnoreCase()(*field_name, *projected_name))
			{
				final_field_name = aliased_filed_name;
			}
		}
		else if(field_name && _alias_to_group_by_field_name.find(*field_name) != _alias_to_group_by_field_name.end())
		{
			auto& aliased_filed_name = _alias_to_group_by_field_name.at(*field_name);
			final_field_name = aliased_filed_name;
		}
		select_tokens.push_back(tokens::GroupByKeyToken::create(final_field_name, projected_name));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_group_by_sum(const std::string& field_name,
		const std::optional<std::string>& projected_name)
	{
		assert_no_raw_query();
		is_group_by = true;

		auto&& valid_field_name = ensure_valid_field_name(field_name, false);
		select_tokens.push_back(tokens::GroupBySumToken::create(valid_field_name, projected_name));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_group_by_count(const std::optional<std::string>& projected_name)
	{
		assert_no_raw_query();
		is_group_by = true;

		select_tokens.push_back(tokens::GroupByCountToken::create(projected_name));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_where_true()
	{
		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, {});

		tokens.push_back(tokens::TrueToken::INSTANCE);
	}

	template <typename T>
	queries::more_like_this::MoreLikeThisScope AbstractDocumentQuery<T>::_more_like_this()
	{
		append_operator_if_needed(where_tokens);

		where_tokens.push_back(std::static_pointer_cast<tokens::QueryToken>(tokens::MoreLikeThisToken::create()));
		return queries::more_like_this::MoreLikeThisScope(where_tokens.back(), 
			[this](nlohmann::json value)->std::string
		{
			return add_query_parameter(value);
		}, 
			[this]()->void
		{
			_is_more_like_this = false;
		});
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_include(std::string path)
	{
		document_includes.insert(std::move(path));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_include(std::shared_ptr<loaders::IncludeBuilderBase> includes)
	{
		if(!includes)
		{
			return;
		}

		if(!includes->documents_to_include.empty())
		{
			for(const auto& doc : includes->documents_to_include)
			{
				document_includes.insert(doc);
			}
		}

		//TODO
		//_includeCounters(includes.alias, includes.countersToIncludeBySourcePath);
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
	void AbstractDocumentQuery<T>::_where_lucene(const std::string& field_name, const std::string& where_clause,
		bool exact)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		std::optional<tokens::WhereToken::WhereOptions> options{};
		if(exact)
		{
			options.emplace(exact);
		}
		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::LUCENE, valid_field_name,
			add_query_parameter(where_clause), options);
		tokens.push_back(where_token);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_open_subclause()
	{
		++_current_clause_depth;

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, {});

		tokens.push_back(tokens::OpenSubclauseToken::INSTANCE);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_close_subclause()
	{
		--_current_clause_depth;
		get_current_where_tokens().push_back(tokens::CloseSubclauseToken::INSTANCE);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_equals(std::string field_name, const TValue* value, bool exact)
	{
		auto params = WhereParams<TValue>();
		params.field_name = std::move(field_name);
		params.value = value;
		params.exact = exact;
		_where_equals(std::move(params));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_where_equals(std::string field_name, const MethodCall* method, bool exact)
	{
		_where_equals<MethodCall>(std::move(field_name), method, exact);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_equals(WhereParams<TValue> where_params)
	{
		if(negate)
		{
			negate = false;
			_where_not_equals(std::move(where_params));
			return;
		}

		where_params.field_name = ensure_valid_field_name(where_params.field_name, where_params.nested_path);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);

		if(if_value_is_method(tokens::WhereOperator::EQUALS, where_params, tokens))
		{
			return;
		}

		auto transform_to_equal_value = transform_value(where_params);
		auto add_query_parameters = add_query_parameter(std::move(transform_to_equal_value));
		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::EQUALS, std::move(where_params.field_name),
			std::move(add_query_parameters), tokens::WhereToken::WhereOptions(where_params.exact));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_not_equals(std::string field_name, const TValue* value, bool exact)
	{
		auto params = WhereParams<TValue>();
		params.field_name = std::move(field_name);
		params.value = value;
		params.exact = exact;
		_where_not_equals(std::move(params));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_where_not_equals(std::string field_name, const MethodCall* method,
		bool exact)
	{
		_where_not_equals<MethodCall>(std::move(field_name), method, exact);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_not_equals(WhereParams<TValue> where_params)
	{
		if(negate)
		{
			negate = false;
			_where_equals(std::move(where_params));
			return;
		}

		auto transform_to_equal_value = transform_value(where_params);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);

		where_params.field_name = ensure_valid_field_name(where_params.field_name, where_params.nested_path);

		if (if_value_is_method(tokens::WhereOperator::NOT_EQUALS, where_params, tokens))
		{
			return;
		}
				
		auto add_query_parameters = add_query_parameter(std::move(transform_to_equal_value));
		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::NOT_EQUALS, std::move(where_params.field_name),
			std::move(add_query_parameters), tokens::WhereToken::WhereOptions(where_params.exact));
		tokens.push_back(where_token);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::negate_next()
	{
		negate = !negate;
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_in(const std::string& field_name,
		const std::vector<TValue>& values, bool exact)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		auto optional_field_name = std::optional<std::string>();
		optional_field_name = valid_field_name;
		negate_if_needed(tokens, optional_field_name);

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::_IN, std::move(valid_field_name),
			add_query_parameter(transform_collection(field_name, values)));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_starts_with(std::string field_name, const TValue* value)
	{
		auto where_params = WhereParams<TValue>();
		where_params.field_name = std::move(field_name);
		where_params.value = value;
		where_params.allow_wildcards = true;

		auto transform_to_equal_value = transform_value(where_params);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);

		where_params.field_name = ensure_valid_field_name(where_params.field_name, where_params.nested_path);
		negate_if_needed(tokens, where_params.field_name);

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::STARTS_WITH, std::move(where_params.field_name),
			add_query_parameter(std::move(transform_to_equal_value)));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_ends_with(std::string field_name, const TValue* value)
	{
		auto where_params = WhereParams<TValue>();
		where_params.field_name = std::move(field_name);
		where_params.value = value;
		where_params.allow_wildcards = true;

		auto transform_to_equal_value = transform_value(where_params);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);

		where_params.field_name = ensure_valid_field_name(where_params.field_name, where_params.nested_path);
		negate_if_needed(tokens, where_params.field_name);

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::ENDS_WITH, std::move(where_params.field_name),
			add_query_parameter(std::move(transform_to_equal_value)));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_between(const std::string& field_name,const TValue* start,
		const TValue* end, bool exact)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		auto start_params = WhereParams<TValue>();
		start_params.value = start;
		start_params.field_name = valid_field_name;

		auto end_params = WhereParams<TValue>();
		end_params.value = end;
		start_params.field_name = valid_field_name;

		auto from_parameter_name = add_query_parameter(!start ? "*" : transform_value(start_params, true));
		auto to_parameter_name = add_query_parameter(!end ? "NULL" : transform_value(end_params, true));

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::BETWEEN, valid_field_name , {},
			tokens::WhereToken::WhereOptions(exact, std::move(from_parameter_name), std::move(to_parameter_name)));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_greater_than(const std::string& field_name,const TValue* value,
		bool exact)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		auto where_params = WhereParams<TValue>();
		where_params.value = value;
		where_params.field_name = valid_field_name;

		auto parameter = add_query_parameter(!value ? "*" : transform_value(where_params, true));

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::GREATER_THAN, valid_field_name,
			std::move(parameter), tokens::WhereToken::WhereOptions(exact));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_greater_than_or_equal(const std::string& field_name,
		const TValue* value, bool exact)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		auto where_params = WhereParams<TValue>();
		where_params.value = value;
		where_params.field_name = valid_field_name;

		auto parameter = add_query_parameter(!value ? "*" : transform_value(where_params, true));

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::GREATER_THAN_OR_EQUAL, valid_field_name,
			std::move(parameter), tokens::WhereToken::WhereOptions(exact));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_less_than(const std::string& field_name, const TValue* value,
		bool exact)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		auto where_params = WhereParams<TValue>();
		where_params.value = value;
		where_params.field_name = valid_field_name;

		auto parameter = add_query_parameter(!value ? "NULL" : transform_value(where_params, true));

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::LESS_THAN, valid_field_name,
			std::move(parameter), tokens::WhereToken::WhereOptions(exact));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_where_less_than_or_equal(const std::string& field_name,
		const TValue* value, bool exact)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		auto where_params = WhereParams<TValue>();
		where_params.value = value;
		where_params.field_name = valid_field_name;

		auto parameter = add_query_parameter(!value ? "NULL" : transform_value(where_params, true));

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::LESS_THAN_OR_EQUAL, valid_field_name,
			std::move(parameter), tokens::WhereToken::WhereOptions(exact));
		tokens.push_back(where_token);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_where_regex(const std::string& field_name, std::string pattern)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		auto where_params = WhereParams<std::string>();
		where_params.value = &pattern;
		where_params.field_name = valid_field_name;

		auto parameter = add_query_parameter(transform_value(where_params));

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::REGEX, valid_field_name, std::move(parameter));
		tokens.push_back(where_token);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_and_also()
	{
		auto& tokens = get_current_where_tokens();
		if(tokens.empty())
		{
			return;
		}

		if(std::dynamic_pointer_cast<tokens::QueryOperatorToken>(tokens.back()))
		{
			throw std::runtime_error("Cannot add AND, previous token was already an operator token.");
		}

		tokens.push_back(tokens::QueryOperatorToken::AND);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_or_else()
	{
		auto& tokens = get_current_where_tokens();
		if (tokens.empty())
		{
			return;
		}

		if (std::dynamic_pointer_cast<tokens::QueryOperatorToken>(tokens.back()))
		{
			throw std::runtime_error("Cannot add OR, previous token was already an operator token.");
		}

		tokens.push_back(tokens::QueryOperatorToken::OR);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_boost(double boost)
	{
		if(boost == 1.0)
		{
			return;
		}

		auto& tokens = get_current_where_tokens();
		if(tokens.empty())
		{
			throw std::runtime_error("Missing where clause");
		}

		auto& last_token = tokens.back();
		auto where_token = std::dynamic_pointer_cast<tokens::WhereToken>(last_token);
		if(!where_token)
		{
			throw std::runtime_error("Missing where clause");
		}

		if (boost < 0.0)
		{
			throw std::invalid_argument("Boost factor must be a positive number");
		}

		where_token->options.boost = boost;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_fuzzy(double fuzzy)
	{
		auto& tokens = get_current_where_tokens();
		if (tokens.empty())
		{
			throw std::runtime_error("Fuzzy can only be used right after where clause");
		}

		auto& last_token = tokens.back();
		auto where_token = std::dynamic_pointer_cast<tokens::WhereToken>(last_token);
		if (!where_token)
		{
			throw std::runtime_error("Fuzzy can only be used right after where clause");
		}

		if(where_token->where_operator != tokens::WhereOperator::EQUALS)
		{
			throw std::runtime_error("Fuzzy can only be used right after where clause with equals operator");
		}

		if (fuzzy < 0.0 || fuzzy > 1.0)
		{
			throw std::invalid_argument("Fuzzy distance must be between 0.0 and 1.0");
		}

		where_token->options.fuzzy = fuzzy;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_proximity(int32_t proximity)
	{
		auto& tokens = get_current_where_tokens();
		if (tokens.empty())
		{
			throw std::runtime_error("Proximity can only be used right after search clause");
		}

		auto& last_token = tokens.back();
		auto where_token = std::dynamic_pointer_cast<tokens::WhereToken>(last_token);
		if (!where_token)
		{
			throw std::runtime_error("Proximity can only be used right after search clause");
		}

		if (where_token->where_operator != tokens::WhereOperator::SEARCH)
		{
			throw std::runtime_error("Proximity can only be used right after search clause");
		}

		if (proximity < 1)
		{
			throw std::invalid_argument("Proximity can only be used right after search clause");
		}

		where_token->options.proximity = proximity;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_wait_for_non_stale_results(const std::optional<std::chrono::milliseconds>& wait_timeout)
	{
		the_wait_for_non_stale_results = true;
		timeout = wait_timeout ? *wait_timeout : DEFAULT_TIMEOUT;
	}

	template <typename T>
	std::vector<std::string> AbstractDocumentQuery<T>::get_projection_fields() const
	{
		if(fields_to_fetch_token && fields_to_fetch_token->_projections)
		{
			return fields_to_fetch_token->_projections.value();
		}
		return {};
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_random_ordering()
	{
		assert_no_raw_query();

		_no_caching();
		order_by_tokens.push_back(tokens::OrderByToken::random);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_random_ordering(const std::string& seed)
	{
		assert_no_raw_query();

		if(impl::utils::is_blank(seed))
		{
			_random_ordering();
		}
		else
		{
			_no_caching();
			order_by_tokens.push_back(tokens::OrderByToken::create_random(seed));
		}
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
		assert_no_raw_query();
		auto valid_field = ensure_valid_field_name(field, false);
		order_by_tokens.push_back(tokens::OrderByToken::create_ascending(std::move(valid_field), ordering));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_descending(const std::string& field, OrderingType ordering)
	{
		assert_no_raw_query();
		auto valid_field = ensure_valid_field_name(field, false);
		order_by_tokens.push_back(tokens::OrderByToken::create_descending(std::move(valid_field), ordering));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_score()
	{
		assert_no_raw_query();
		order_by_tokens.push_back(tokens::OrderByToken::score_ascending);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_score_descending()
	{
		assert_no_raw_query();
		order_by_tokens.push_back(tokens::OrderByToken::score_descending);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_statistics(std::shared_ptr<QueryStatistics>& stats) const
	{
		stats = query_stats;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::invoke_after_query_executed(const queries::QueryResult& result)
	{
		primitives::EventHelper::invoke(after_query_executed_callback, result);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::invoke_before_query_executed(const queries::IndexQuery& query)
	{
		primitives::EventHelper::invoke(before_query_executed_callback, query);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::invoke_after_stream_executed(const nlohmann::json& result)
	{
		primitives::EventHelper::invoke(after_stream_executed_callback, result);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_search(const std::string& field_name, std::string search_terms,
		queries::SearchOperator search_operator)
	{
		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);

		auto valid_field_name = ensure_valid_field_name(field_name, false);
		negate_if_needed(tokens, valid_field_name);

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::SEARCH, std::move(valid_field_name),
			add_query_parameter(std::move(search_terms)), tokens::WhereToken::WhereOptions(search_operator));
		tokens.push_back(where_token);
	}

	template <typename T>
	AbstractDocumentQuery<T>::AbstractDocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::optional<std::string> index_name_param,
		std::optional<std::string> collection_name_param,
		bool is_group_by_param,
		std::shared_ptr<tokens::DeclareToken> declare_token_param,
		std::vector<std::shared_ptr<tokens::LoadToken>> load_tokens_param,
		std::optional<std::string> from_alias)
		: index_name(std::move(index_name_param))
		, collection_name(std::move(collection_name_param))
		, is_group_by(is_group_by_param)
		, the_session(session)
		, from_token(tokens::FromToken::create(index_name, collection_name, std::move(from_alias)))
		, declare_token(declare_token_param)
		, load_tokens(std::move(load_tokens_param))
	{
		root_types.insert(typeid(T));
		_conventions = session->get_conventions() ? session->get_conventions() : conventions::DocumentConventions::create();
	}

	template <typename T>
	void AbstractDocumentQuery<T>::complete_construction()
	{
		_add_after_query_executed_listener([weak_this = _weak_this](const queries::QueryResult& query_result)
		{
			if(auto shared_this = weak_this.lock())
			{
				shared_this->update_stats_highlightings_and_explanations(query_result);
			}
		});
	}

	template <typename T>
	std::shared_ptr<operations::QueryOperation> AbstractDocumentQuery<T>::initialize_query_operation()
	{
		auto&& index_query = get_index_query();

		return std::make_shared<operations::QueryOperation>(the_session.lock(), index_name, index_query, fields_to_fetch_token, disable_entities_tracking,
			false, false);
	}

	template <typename T>
	queries::IndexQuery AbstractDocumentQuery<T>::generate_index_query(std::string query) const
	{
		auto index_query = queries::IndexQuery(std::move(query));
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
	void AbstractDocumentQuery<T>::update_fields_to_fetch_token(
		std::shared_ptr<tokens::FieldsToFetchToken> fields_to_fetch)
	{
		fields_to_fetch_token = fields_to_fetch;

		if(select_tokens.empty())
		{
			select_tokens.push_back(fields_to_fetch);
		}
		else
		{
			if(auto it = std::find_if(select_tokens.begin(), select_tokens.end(), 
				[](std::shared_ptr<tokens::QueryToken> token)
			{
				return std::dynamic_pointer_cast<tokens::FieldsToFetchToken>(token);
			});
				it != select_tokens.end())
			{
				*it = fields_to_fetch;
			}else
			{
				select_tokens.push_back(fields_to_fetch);
			}
		}
	}

	template <typename T>
	template <typename Type>
	void AbstractDocumentQuery<T>::get_source_alias_if_exists(
		const queries::QueryData& query_data, const std::vector<std::string>& fields,
		std::optional<std::string>& source_alias_reference)
	{
		if(fields.size() != 1)
		{
			source_alias_reference.reset();
			return;
		}

		auto index = fields[0].find(".");
		if(index == std::string::npos)
		{
			source_alias_reference.reset();
			return;
		}

		auto possible_alias = fields[0].substr(0, index);
		if(query_data.from_alias && query_data.from_alias.value() == possible_alias)
		{
			source_alias_reference.emplace(std::move(possible_alias));
			return;
		}
		if(query_data.load_tokens.empty())
		{
			source_alias_reference.reset();
			return ;
		}
		if(std::all_of(query_data.load_tokens.begin(), query_data.load_tokens.end(), 
			[&](std::shared_ptr<tokens::LoadToken> token)
		{
			return !token->alias || token->alias.value() != possible_alias;
		}))
		{
			source_alias_reference.reset();
			return;
		}

		source_alias_reference.emplace(std::move(possible_alias));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_include_timings(std::shared_ptr<queries::timings::QueryTimings>& timings)
	{
		if(!query_timings)
		{
			query_timings = std::make_shared<queries::timings::QueryTimings>();
		}

		timings = query_timings;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_within_radius_of(const std::string& field_name, double radius, double latitude,
		double longitude, const std::optional<indexes::spatial::SpatialUnits>& radius_units, double dist_error_percent)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		auto&& radius_param = add_query_parameter(radius);
		auto&& latitude_param = add_query_parameter(latitude);
		auto&& longitude_param = add_query_parameter(longitude);

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::SPATIAL_WITHIN, std::move(valid_field_name),
			{}, tokens::WhereToken::WhereOptions(tokens::ShapeToken::circle(
				radius_param, latitude_param, longitude_param,
				radius_units), dist_error_percent));
		tokens.push_back(where_token);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_spatial(const std::string& field_name, const std::string& shape_wkt,
		indexes::spatial::SpatialRelation relation, const std::optional<indexes::spatial::SpatialUnits>& units,
		double dist_error_percent)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		auto wkt_token = tokens::ShapeToken::wkt(add_query_parameter(shape_wkt), units);

		tokens::WhereOperator where_operator{};
		switch (relation)
		{
		case indexes::spatial::SpatialRelation::WITHIN:
			where_operator = tokens::WhereOperator::SPATIAL_WITHIN;
			break;
		case indexes::spatial::SpatialRelation::CONTAINS:
			where_operator = tokens::WhereOperator::SPATIAL_CONTAINS;
			break;
		case indexes::spatial::SpatialRelation::DISJOINT:
			where_operator = tokens::WhereOperator::SPATIAL_DISJOINT;
			break;
		case indexes::spatial::SpatialRelation::INTERSECTS:
			where_operator = tokens::WhereOperator::SPATIAL_INTERSECTS;
			break;
		default:
			throw std::invalid_argument("argument 'relation' is of unknown type");
		}

		tokens.push_back(tokens::WhereToken::create(where_operator, valid_field_name, {},
			tokens::WhereToken::WhereOptions(wkt_token, dist_error_percent)));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_spatial(const queries::spatial::DynamicSpatialField& dynamic_field,
		const queries::spatial::SpatialCriteria& criteria)
	{
		assert_is_dynamic_query(dynamic_field, "spatial");

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, {});

		tokens.push_back(criteria.to_query_token(dynamic_field.to_field([this](const std::string& field_name, bool is_nested_path)
		{
			return ensure_valid_field_name(field_name, is_nested_path);
		}), [this](nlohmann::json value)
		{
			return add_query_parameter(std::move(value));
		}));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_spatial(const std::string& field_name,
		const queries::spatial::SpatialCriteria& criteria)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, valid_field_name);

		tokens.push_back(criteria.to_query_token(std::move(valid_field_name), [this](nlohmann::json value)
		{
			return add_query_parameter(std::move(value));
		}));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_distance(const queries::spatial::DynamicSpatialField& field,
		double latitude, double longitude)
	{
		assert_is_dynamic_query(field, "order_by_distance");

		std::ostringstream field_name{};
		field_name << "'" << field.to_field([this](const std::string& field_name, bool is_nested_path)
		{
			return ensure_valid_field_name(field_name, is_nested_path);
		})
			<< "'";
		_order_by_distance(field_name.str(), latitude, longitude);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_distance(const std::string& field_name, double latitude, double longitude)
	{
		auto&& latitude_param = add_query_parameter(latitude);
		auto && longitude_param = add_query_parameter(longitude);

		order_by_tokens.push_back(tokens::OrderByToken::create_distance_ascending(field_name,
			latitude_param, longitude_param));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_distance(const queries::spatial::DynamicSpatialField& field,
		const std::string& shape_wkt)
	{
		assert_is_dynamic_query(field, "order_by_distance");

		std::ostringstream field_name{};
		field_name << "'" << field.to_field([this](const std::string& field_name, bool is_nested_path)
		{
			return ensure_valid_field_name(field_name, is_nested_path);
		})
			<< "'";
		_order_by_distance(field_name.str(), shape_wkt);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_distance(const std::string& field_name, const std::string& shape_wkt)
	{
		order_by_tokens.push_back(tokens::OrderByToken::create_distance_ascending(field_name, add_query_parameter(shape_wkt)));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_distance_descending(const queries::spatial::DynamicSpatialField& field,
		double latitude, double longitude)
	{
		assert_is_dynamic_query(field, "order_by_distance_descending");

		std::ostringstream field_name{};
		field_name << "'" << field.to_field([this](const std::string& field_name, bool is_nested_path)
		{
			return ensure_valid_field_name(field_name, is_nested_path);
		})
			<< "'";
		_order_by_distance_descending(field_name.str(), latitude, longitude);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_distance_descending(const std::string& field_name, double latitude,
		double longitude)
	{
		order_by_tokens.push_back(tokens::OrderByToken::create_distance_descending(field_name,
			add_query_parameter(latitude), add_query_parameter(longitude)));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_distance_descending(const queries::spatial::DynamicSpatialField& field,
		const std::string& shape_wkt)
	{
		assert_is_dynamic_query(field, "order_by_distance_descending");

		std::ostringstream field_name{};
		field_name << "'" << field.to_field([this](const std::string& field_name, bool is_nested_path)
		{
			return ensure_valid_field_name(field_name, is_nested_path);
		})
			<< "'";
		_order_by_distance_descending(field_name.str(), shape_wkt);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_order_by_distance_descending(const std::string& field_name,
		const std::string& shape_wkt)
	{
		order_by_tokens.push_back(tokens::OrderByToken::create_distance_descending(field_name, add_query_parameter(shape_wkt)));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_highlight(std::string field_name, int32_t fragment_length, int32_t fragment_count,
		const std::optional<queries::highlighting::HighlightingOptions>& options,
		std::shared_ptr<queries::highlighting::Highlightings>& highlightings_reference)
	{
		highlightings_reference = query_highlightings.add(field_name);
		std::optional<std::string> options_parameter_name{};
		if (options)
		{
			options_parameter_name.emplace(add_query_parameter(*options));
		}
		highlighting_tokens.push_back(tokens::HighlightingToken::create(std::move(field_name), fragment_length, fragment_count,
			std::move(options_parameter_name)));
	}

	template <typename T>
	void AbstractDocumentQuery<T>::negate_if_needed(std::list<std::shared_ptr<tokens::QueryToken>>& tokens,
		const std::optional<std::string>& field_name)
	{
		if(!negate)
		{
			return;
		}

		negate = false;

		if(tokens.empty() || std::dynamic_pointer_cast<tokens::OpenSubclauseToken>(tokens.back()))
		{
			if(field_name)
			{
				_where_exists(*field_name);
			}
			else
			{
				_where_true();
			}
			_and_also();
		}

		tokens.push_back(tokens::NegateToken::INSTANCE);
	}

	template <typename T>
	template <typename TValue>
	bool AbstractDocumentQuery<T>::if_value_is_method(tokens::WhereOperator op, const WhereParams<TValue>& where_params,
		std::list<std::shared_ptr<tokens::QueryToken>>& tokens)
	{
		if constexpr (std::is_base_of_v<MethodCall, TValue>)
		{
			const MethodCall* mc = static_cast<const MethodCall*>(where_params.value);

			std::vector<std::string> args{};
			args.reserve(mc->args.size());
			for(const auto& arg : mc->args)
			{
				args.push_back(add_query_parameter(arg));
			}

			std::shared_ptr<tokens::WhereToken> token{};
			if(dynamic_cast<const CmpXchg*>(mc) != nullptr)
			{
				token = tokens::WhereToken::create(op, where_params.field_name, {},
					tokens::WhereToken::WhereOptions(tokens::WhereToken::MethodsType::CMP_X_CHG, std::move(args),
						mc->access_path, where_params.exact));
			}
			else
			{
				throw std::invalid_argument("Unknown method");
			}

			tokens.push_back(token);
			return true;
		}

		return false;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::build_include(std::ostringstream& writer) const
	{
		if(document_includes.empty() &&
			highlighting_tokens.empty() &&
			!explanation_token &&
			!query_timings &&
			/*!counter_includes_tokens*/true)
		{
			return;
		}

		writer << " include ";
		bool first = true;
		for(const auto& include : document_includes)
		{
			if(!first)
			{
				writer << ",";
			}
			first = false;

			bool required_quotes = false;

			for(auto ch : include)
			{
				if(!std::isalnum((unsigned char)ch) && ch != '_' && ch != '.')
				{
					required_quotes = true;
					break;
				}
			}

			if(required_quotes)
			{
				writer << "'";
				std::transform(include.cbegin(), include.cend(), std::ostream_iterator<std::string>(writer),
					[](auto c)->std::string
				{
					return c == '\'' ? "\\'" : std::string{ c };
				});
				writer << "'";
			}
			else
			{
				writer << include;
			}
		}

		//TODO
		// if (counterIncludesTokens != null) 

		for(auto& token : highlighting_tokens)
		{
			if(!first)
			{
				writer << ",";
			}
			first = false;

			token->write_to(writer);
		}

		if(explanation_token)
		{
			if (!first)
			{
				writer << ",";
			}
			first = false;
			explanation_token->write_to(writer);
		}

		if (query_timings)
		{
			if (!first)
			{
				writer << ",";
			}
			first = false;
			tokens::TimingsToken::INSTANCE->write_to(writer);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::update_stats_highlightings_and_explanations(const queries::QueryResult& query_result)
	{
		query_stats->update_query_stats(query_result);
		query_highlightings.update(query_result);
		if(explanations)
		{
			explanations->update(query_result);
		}
		if(query_timings)
		{
			query_timings->update(query_result);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::build_select(std::ostringstream& writer) const
	{
		if(select_tokens.empty())
		{
			return;
		}

		writer << " select ";
		if(select_tokens.size() == 1 && std::dynamic_pointer_cast<tokens::DistinctToken>(select_tokens.front()))
		{
			select_tokens.front()->write_to(writer);
			writer << " *";
			return;
		}

		for(auto it = select_tokens.begin(); it != select_tokens.end(); ++it)
		{
			if(it != select_tokens.begin() && !std::dynamic_pointer_cast<tokens::DistinctToken>(*std::prev(it)))
			{
				writer << ",";
			}
			DocumentQueryHelper::add_space_if_needed(it != select_tokens.begin() ? *std::prev(it) : std::shared_ptr<tokens::QueryToken>(),
				*it, writer);
			(*it)->write_to(writer);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::init_sync()
	{
		if (query_operation)
		{
			return;
		}

		//TODO
		//BeforeQueryEventArgs beforeQueryEventArgs = new BeforeQueryEventArgs(theSession, new DocumentQueryCustomizationDelegate(this));
		//theSession.onBeforeQueryInvoke(beforeQueryEventArgs);

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
	bool AbstractDocumentQuery<T>::is_distinct() const
	{
		return !select_tokens.empty() &&
			std::dynamic_pointer_cast<tokens::DistinctToken>(select_tokens.front());
	}

	template <typename T>
	std::shared_ptr<conventions::DocumentConventions> AbstractDocumentQuery<T>::get_conventions() const
	{
		return _conventions;
	}

	template <typename T>
	std::shared_ptr<DocumentSessionImpl> AbstractDocumentQuery<T>::get_session() const
	{

		return std::reinterpret_pointer_cast<DocumentSessionImpl>(the_session.lock());
	}

	template <typename T>
	bool AbstractDocumentQuery<T>::is_dynamic_map_reduce() const
	{
		return !group_by_tokens.empty();
	}

	template <typename T>
	std::shared_ptr<operations::QueryOperation> AbstractDocumentQuery<T>::get_query_operation() const
	{
		return query_operation;
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_add_before_query_executed_listener(std::function<void(const queries::IndexQuery&)> action)
	{
		before_query_executed_callback.push_back(action);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_remove_before_query_executed_listener(
		std::function<void(const queries::IndexQuery&)> action)
	{
		if (auto it = std::find(before_query_executed_callback.begin(), before_query_executed_callback.end(), action);
			it != before_query_executed_callback.end())
		{
			before_query_executed_callback.erase(it);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_add_after_query_executed_listener(std::function<void(const queries::QueryResult&)> action)
	{
		after_query_executed_callback.push_back(action);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_remove_after_query_executed_listener(std::function<void(const queries::QueryResult&)> action)
	{
		if (auto it = std::find(after_query_executed_callback.begin(), after_query_executed_callback.end(), action);
			it != after_query_executed_callback.end())
		{
			after_query_executed_callback.erase(it);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_add_after_stream_executed_listener(
		std::function<void(const nlohmann::json&)> action)
	{
		after_stream_executed_callback.push_back(action);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_remove_after_stream_executed_listener(
		std::function<void(const nlohmann::json&)> action)
	{
		if (auto it = std::find(after_stream_executed_callback.begin(), after_stream_executed_callback.end(), action);
			it != after_stream_executed_callback.end())
		{
			after_stream_executed_callback.erase(it);
		}
	}

	template <typename T>
	std::vector<std::shared_ptr<T>> AbstractDocumentQuery<T>::to_list(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return execute_query_operation({}, from_json);
	}

	template <typename T>
	int32_t AbstractDocumentQuery<T>::count()
	{
		_take(0);
		return get_query_result().total_results;
	}

	template <typename T>
	bool AbstractDocumentQuery<T>::any()
	{
		if(is_distinct())
		{	// for distinct it is cheaper to do count 1
			return !execute_query_operation(1).empty();
		}

		_take(0);
		auto&& query_result = get_query_result();
		return query_result.total_results > 0;
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
			the_session.lock()->get_conventions(),
			query_operation,
			after_query_executed_callback);

		auto get_operation_result = [=](std::shared_ptr<operations::lazy::ILazyOperation> operation)
		{
			auto lazy_query_op = std::static_pointer_cast<operations::lazy::LazyQueryOperation<T>>(operation);
			return lazy_query_op->get_result();
		};

		return get_session()->template add_lazy_operation<std::vector<std::shared_ptr<T>>>(lazy_query_operation,
			get_operation_result, [=]()
		{
			if (on_eval)
				(*on_eval)(get_operation_result(std::static_pointer_cast<operations::lazy::ILazyOperation>(lazy_query_operation)));
		});
	}

	template <typename T>
	Lazy<int32_t> AbstractDocumentQuery<T>::count_lazily()
	{
		if (!get_query_operation())
		{
			_take(0);
			query_operation = initialize_query_operation();
		}

		auto lazy_query_operation = std::make_shared<operations::lazy::LazyQueryOperation<T>>(
			the_session.lock()->get_conventions(),
			query_operation,
			after_query_executed_callback);

		return get_session()->add_lazy_count_operation(lazy_query_operation);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_suggest_using(const queries::suggestions::SuggestionBase& suggestion)
	{
		assert_can_suggest();

		auto token = std::shared_ptr<tokens::SuggestToken>();

		if(const auto* term = dynamic_cast<const queries::suggestions::SuggestionWithTerm*>(&suggestion);
			term != nullptr)
		{
			token = tokens::SuggestToken::create(term->field, add_query_parameter(term->term),
				get_options_parameter_name(term->options));
		}
		else if (const auto* terms = dynamic_cast<const queries::suggestions::SuggestionWithTerms*>(&suggestion);
			term != nullptr)
		{
			token = tokens::SuggestToken::create(terms->field, add_query_parameter(terms->terms),
				get_options_parameter_name(terms->options));
		}
		else
		{
			throw std::invalid_argument("Unknown type of suggestion");
		}

		select_tokens.push_back(token);
	}

	template <typename T>
	std::optional<std::string> AbstractDocumentQuery<T>::get_options_parameter_name(
		const std::optional<queries::suggestions::SuggestionOptions>& options)
	{
		std::optional<std::string> options_parameter_name{};
		if(options && *options != queries::suggestions::SuggestionOptions::default_options)
		{
			options_parameter_name.emplace(add_query_parameter(*options));
		}

		return std::move(options_parameter_name);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_include_explanations(const std::optional<queries::explanation::ExplanationOptions>& options,
		std::shared_ptr<queries::explanation::Explanations>& explanations_reference)
	{
		if(explanation_token)
		{
			throw std::runtime_error("Duplicate 'include_explanations' method calls are forbidden.");
		}

		explanation_token = tokens::ExplanationToken::create(options ? add_query_parameter(*options) :
			std::optional<std::string>());
		explanations_reference = explanations = std::make_shared<queries::explanation::Explanations>();
	}

	template <typename T>
	AbstractDocumentQuery<T>::operator std::string() const
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
		build_group_by(query_text);
		build_where(query_text);
		build_order_by(query_text);

		build_load(query_text);
		build_select(query_text);
		build_include(query_text);

		return query_text.str();
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_intersect()
	{
		auto& tokens = get_current_where_tokens();
		if(!tokens.empty())
		{
			auto& last_token = tokens.back();
			if(std::dynamic_pointer_cast<tokens::WhereToken>(last_token) ||
				std::dynamic_pointer_cast<tokens::CloseSubclauseToken>(last_token))
			{
				is_intersect = true;
				tokens.push_back(tokens::IntersectMarkerToken::INSTANCE);
				return;
			}
		}

		throw std::runtime_error("Cannot add INTERSECT at this point.");
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_where_exists(const std::string& field_name)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, field_name);

		tokens.push_back(tokens::WhereToken::create(tokens::WhereOperator::EXISTS, field_name, {}));
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_contains_any(const std::string& field_name,
		const std::vector<TValue>& values)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, field_name);

		auto array = transform_collection(field_name, values);

		auto where_token = tokens::WhereToken::create(tokens::WhereOperator::_IN, field_name, 
			add_query_parameter(std::move(array)), tokens::WhereToken::WhereOptions(false));
		tokens.push_back(where_token);
	}

	template <typename T>
	template <typename TValue>
	void AbstractDocumentQuery<T>::_contains_all(const std::string& field_name,
		const std::vector<TValue>& values)
	{
		auto valid_field_name = ensure_valid_field_name(field_name, false);

		auto& tokens = get_current_where_tokens();
		append_operator_if_needed(tokens);
		negate_if_needed(tokens, field_name);

		auto array = transform_collection(field_name, values);

		if(array.empty())
		{
			tokens.push_back(tokens::TrueToken::INSTANCE);
		}
		else
		{
			auto where_token = tokens::WhereToken::create(tokens::WhereOperator::ALL_IN, field_name,
				add_query_parameter(std::move(array)));
			tokens.push_back(where_token);
		}
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_add_root_type(std::type_index type)
	{
		root_types.insert(type);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::_distinct()
	{
		if(is_distinct())
		{
			throw std::runtime_error("This is already a distinct query");
		}

		select_tokens.push_front(tokens::DistinctToken::INSTANCE);
	}

	template <typename T>
	void AbstractDocumentQuery<T>::add_from_alias_to_where_token(const std::string& from_alias)
	{
		if(from_alias.empty())
		{
			throw std::invalid_argument("Alias cannot be empty");
		}

		auto& tokens = get_current_where_tokens();

		for(auto& token : tokens)
		{
			if(auto where_token = std::dynamic_pointer_cast<tokens::WhereToken>(token);
				where_token)
			{
				where_token->add_alias(from_alias);
			}
		}
	}
}
