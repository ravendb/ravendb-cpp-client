#pragma once
#include "AbstractDocumentQuery.h"
#include "IDocumentQuery.h"
#include "SpatialCriteriaFactory.h"
#include "MoreLikeThisUsingDocument.h"
#include "GroupByDocumentQuery.h"
#include "GetTypeProjectionFields.h"

namespace ravendb::client::documents::session
{
	template<typename T>
	class DocumentQuery : public AbstractDocumentQuery<T>, public IDocumentQuery<T, DocumentQuery<T>>
	{
	public:
		template<typename U>
		friend class DocumentQuery;

	private:
		DocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::optional<std::string> index_name,
			std::optional<std::string> collection_name, bool is_group_by, std::shared_ptr<tokens::DeclareToken> declare_token,
			std::vector<std::shared_ptr<tokens::LoadToken>> load_tokens, std::optional<std::string> from_alias);

		template<typename TResult>
		std::shared_ptr<DocumentQuery<TResult>> create_document_query_internal(const std::optional<queries::QueryData>& query_data = {});

	public:
		static std::shared_ptr<DocumentQuery> create(std::shared_ptr<InMemoryDocumentSessionOperations> session,
			std::optional<std::string> index_name,
			std::optional<std::string> collection_name, bool is_group_by, std::shared_ptr<tokens::DeclareToken> declare_token,
			std::vector<std::shared_ptr<tokens::LoadToken>> load_tokens, std::optional<std::string> from_alias);

		static std::shared_ptr<DocumentQuery> create(std::shared_ptr<InMemoryDocumentSessionOperations> session,
			std::optional<std::string> index_name,
			std::optional<std::string> collection_name, bool is_group_by);

		std::shared_ptr<conventions::DocumentConventions> get_conventions() const;

		std::vector<std::shared_ptr<T>> to_list(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		Lazy<std::vector<std::shared_ptr<T>>> lazily(
			const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval);

		template<typename TProjection>
		std::shared_ptr<IDocumentQuery<TProjection, DocumentQuery<TProjection>>> select_fields();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> distinct();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_score();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_score_descending();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> include_explanation(
			std::shared_ptr<queries::explanation::Explanations>& explanations_reference);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> include_explanations(
			const std::optional<queries::explanation::ExplanationOptions>& options,
			std::shared_ptr<queries::explanation::Explanations>& explanations_reference);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> timings(std::shared_ptr<queries::timings::QueryTimings>& timings);

		template<typename TProjection>
		std::shared_ptr<IDocumentQuery<TProjection, DocumentQuery<TProjection>>> select_fields(const std::vector<std::string>& fields);

		template<typename TProjection>
		std::shared_ptr<IDocumentQuery<TProjection, DocumentQuery<TProjection>>> select_fields(const queries::QueryData& query_data);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> wait_for_non_stale_results(const std::optional<std::chrono::milliseconds>& wait_timeout = {});

		queries::IndexQuery get_index_query();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> add_parameter(std::string name, nlohmann::json object);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> add_order(const std::string& field_name,
			bool descending, OrderingType ordering = OrderingType::STRING);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> add_after_query_executed_listener(std::function<void(const queries::QueryResult&)> action);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> remove_after_query_executed_listener(std::function<void(const queries::QueryResult&)> action);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> add_after_stream_executed_listener(std::function<void(const nlohmann::json&)> action);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> remove_after_stream_executed_listener(std::function<void(const nlohmann::json&)> action);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> open_subclause();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> close_subclause();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> search(const std::string& field_name, std::string search_terms,
			queries::SearchOperator search_operator = queries::SearchOperator::OR);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> intersect();

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> contains_any(const std::string& field_name, const std::vector<TValue>& values);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> contains_all(const std::string& field_name, const std::vector<TValue>& values);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> statistics(std::shared_ptr<QueryStatistics>& stats) const;

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> using_default_operator(queries::QueryOperator query_operator);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> no_tracking();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> no_caching();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> include(std::string path);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> include(const std::function<
			void(std::shared_ptr<loaders::IQueryIncludeBuilder<loaders::QueryIncludeBuilder>>)>& includes);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> not_next();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> take(int32_t count);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> skip(int32_t count);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_lucene(const std::string& field_name, const std::string& where_clause, bool exact = false);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_equals(std::string field_name, const TValue& value, bool exact = false);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_equals(std::string field_name, const MethodCall& method, bool exact = false);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_equals(WhereParams<TValue> where_params);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_not_equals(std::string field_name, const TValue& value, bool exact = false);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_not_equals(std::string field_name, const MethodCall& method, bool exact = false);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_not_equals(WhereParams<TValue> where_params);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_in(const std::string& field_name, const std::vector<TValue>& values, bool exact = false);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_starts_with(std::string field_name, const TValue& value);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_ends_with(std::string field_name, const TValue& value);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_between(const std::string& field_name, const TValue& start, const TValue& end,
			bool exact = false);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_greater_than(const std::string& field_name, const TValue& value, bool exact = false);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_greater_than_or_equal(const std::string& field_name, const TValue& value, bool exact = false);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_less_than(const std::string& field_name, const TValue& value, bool exact = false);

		template<typename TValue>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_less_than_or_equal(const std::string& field_name, const TValue& value, bool exact = false);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_exists(const std::string& field_name);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> where_regex(const std::string& field_name, std::string pattern);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> and_also();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> or_else();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> boost(double boost);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> fuzzy(double fuzzy);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> proximity(int32_t proximity);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> random_ordering();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> random_ordering(const std::string& seed);

		template<typename TResult>
		std::shared_ptr<DocumentQuery<TResult>> of_type();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by(const std::string& field,
			OrderingType ordering = OrderingType::STRING);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_descending(const std::string& field,
			OrderingType ordering = OrderingType::STRING);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> add_before_query_executed_listener(
			std::function<void(const queries::IndexQuery&)> action);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> remove_before_query_executed_listener(
			std::function<void(const queries::IndexQuery&)> action);

		Lazy<int32_t> count_lazily();

		std::shared_ptr<T> first(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		std::shared_ptr<T> first_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		std::shared_ptr<T> single(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		std::shared_ptr<T> single_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		bool any();

		int32_t count();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> negate_next();

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> within_radius_of(
			const std::string& field_name, double radius, double latitude, double longitude,
			const std::optional<indexes::spatial::SpatialUnits>& radius_units = {},
			double dist_error_percent = constants::documents::indexing::spatial::DEFAULT_DISTANCE_ERROR_PCT);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> relates_to_shape(const std::string& field_name, const std::string& shape_wkt,
			indexes::spatial::SpatialRelation relation,
			const std::optional<indexes::spatial::SpatialUnits>& units = {},
			double dist_error_percent = constants::documents::indexing::spatial::DEFAULT_DISTANCE_ERROR_PCT);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> spatial(const std::string& field_name,
			std::function<std::unique_ptr<queries::spatial::SpatialCriteria>(const queries::spatial::SpatialCriteriaFactory&)> clause);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> spatial(const queries::spatial::DynamicSpatialField& field_name,
			std::function<std::unique_ptr<queries::spatial::SpatialCriteria>(const queries::spatial::SpatialCriteriaFactory&)> clause);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> more_like_this(const queries::more_like_this::MoreLikeThisBase& more_like_this);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> highlight(std::string field_name, int32_t fragment_length, int32_t fragment_count,
			std::shared_ptr<queries::highlighting::Highlightings>& highlightings_reference);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> highlight(std::string field_name, int32_t fragment_length, int32_t fragment_count,
			const std::optional<queries::highlighting::HighlightingOptions>& options,
			std::shared_ptr<queries::highlighting::Highlightings>& highlightings_reference);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_distance(const queries::spatial::DynamicSpatialField& field,
			double latitude, double longitude);
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_distance(const std::string& field_name,
			double latitude, double longitude);
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_distance(const queries::spatial::DynamicSpatialField& field,
			const std::string& shape_wkt);
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_distance(const std::string& field_name,
			const std::string& shape_wkt);

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_distance_descending(const queries::spatial::DynamicSpatialField& field,
			double latitude, double longitude);
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_distance_descending(const std::string& field_name,
			double latitude, double longitude);
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_distance_descending(const queries::spatial::DynamicSpatialField& field,
			const std::string& shape_wkt);
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> order_by_distance_descending(const std::string& field_name,
			const std::string& shape_wkt);

		const std::optional<std::string>& get_index_name() const;

		bool is_distinct() const;

		queries::QueryResult get_query_result();

		std::shared_ptr<IGroupByDocumentQuery<T, GroupByDocumentQuery<T>>> group_by(const std::vector<std::string>& field_names);

		std::shared_ptr<IGroupByDocumentQuery<T, GroupByDocumentQuery<T>>> group_by(const std::vector<queries::GroupBy>& fields);
	};

	template <typename T>
	DocumentQuery<T>::DocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::optional<std::string> index_name,
		std::optional<std::string> collection_name, bool is_group_by, std::shared_ptr<tokens::DeclareToken> declare_token,
		std::vector<std::shared_ptr<tokens::LoadToken>>  load_tokens, std::optional<std::string> from_alias)
		: AbstractDocumentQuery<T>(session, std::move(index_name), std::move(collection_name), is_group_by,
			declare_token, std::move(load_tokens), std::move(from_alias))
	{}

	template <typename T>
	template <typename TResult>
	std::shared_ptr<DocumentQuery<TResult>> DocumentQuery<T>::create_document_query_internal(
		const std::optional<queries::QueryData>& query_data)
	{
		std::shared_ptr<tokens::FieldsToFetchToken> new_fields_to_fetch{};

		if(query_data && !query_data->fields.empty())
		{
			auto fields = query_data->fields;

			if(!AbstractDocumentQuery<T>::is_group_by)
			{
				if(auto id_helper = get_conventions()->get_entity_id_helper(typeid(TResult));
					id_helper.has_value())
				{
					auto&& identity_field = id_helper->get_id_field_name();
					if(auto it = std::find(fields.begin(), fields.end(), identity_field);
						it != fields.end())
					{
						*it = constants::documents::indexing::fields::DOCUMENT_ID_FIELD_NAME;
					}
				}
			}

			std::optional<std::string> source_alias_reference{};
			AbstractDocumentQuery<T>::template get_source_alias_if_exists<TResult>(*query_data, fields, source_alias_reference);
			new_fields_to_fetch = tokens::FieldsToFetchToken::create(std::move(fields), query_data->projections,
				query_data->is_custom_function, std::move(source_alias_reference));
		}

		if(new_fields_to_fetch)
		{
			AbstractDocumentQuery<T>::update_fields_to_fetch_token(new_fields_to_fetch);
		}

		std::shared_ptr<DocumentQuery<TResult>> query{};
		if(!query_data)
		{
			query = DocumentQuery<TResult>::create(AbstractDocumentQuery<T>::the_session.lock(),
				AbstractDocumentQuery<T>::get_index_name(),
				AbstractDocumentQuery<T>::get_collection_name(),
				AbstractDocumentQuery<T>::is_group_by);
		}
		else
		{
			query = DocumentQuery<TResult>::create(AbstractDocumentQuery<T>::the_session.lock(),
				AbstractDocumentQuery<T>::get_index_name(),
				AbstractDocumentQuery<T>::get_collection_name(),
				AbstractDocumentQuery<T>::is_group_by,
				query_data->declare_token,
				query_data->load_tokens,
				query_data->from_alias);
		}

		query->query_raw = AbstractDocumentQuery<T>::query_raw;
		query->page_size = AbstractDocumentQuery<T>::page_size;
		query->select_tokens = AbstractDocumentQuery<T>::select_tokens;
		query->fields_to_fetch_token = AbstractDocumentQuery<T>::fields_to_fetch_token;
		query->where_tokens = AbstractDocumentQuery<T>::where_tokens;
		query->order_by_tokens = AbstractDocumentQuery<T>::order_by_tokens;
		query->group_by_tokens = AbstractDocumentQuery<T>::group_by_tokens;
		query->query_parameters = AbstractDocumentQuery<T>::query_parameters;
		query->start = AbstractDocumentQuery<T>::start;
		query->timeout = AbstractDocumentQuery<T>::timeout;
		query->query_stats = AbstractDocumentQuery<T>::query_stats;
		query->the_wait_for_non_stale_results = AbstractDocumentQuery<T>::the_wait_for_non_stale_results;
		query->negate = AbstractDocumentQuery<T>::negate;
		query->document_includes = AbstractDocumentQuery<T>::document_includes;
		//TODO query->counter_includes_token = AbstractDocumentQuery<T>::counter_includes_token;
		query->root_types = { std::type_index(typeid(T)) };
		query->before_query_executed_callback = AbstractDocumentQuery<T>::before_query_executed_callback;
		query->after_query_executed_callback = AbstractDocumentQuery<T>::after_query_executed_callback;
		query->after_stream_executed_callback = AbstractDocumentQuery<T>::after_stream_executed_callback;
		query->highlighting_tokens = AbstractDocumentQuery<T>::highlighting_tokens;
		query->query_highlightings = AbstractDocumentQuery<T>::query_highlightings;
		query->disable_entities_tracking = AbstractDocumentQuery<T>::disable_entities_tracking;
		query->disable_caching = AbstractDocumentQuery<T>::disable_caching;
		query->query_timings = AbstractDocumentQuery<T>::query_timings;
		query->explanations = AbstractDocumentQuery<T>::explanations;
		query->explanation_token = AbstractDocumentQuery<T>::explanation_token;
		query->is_intersect = AbstractDocumentQuery<T>::is_intersect;
		query->default_operator = AbstractDocumentQuery<T>::default_operator;

		return query;
	}

	template <typename T>
	std::shared_ptr<DocumentQuery<T>> DocumentQuery<T>::create(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::optional<std::string> index_name, std::optional<std::string> collection_name, bool is_group_by)
	{
		return create(session, std::move(index_name), std::move(collection_name), 
			is_group_by, {}, {}, {});
	}

	template <typename T>
	std::shared_ptr<DocumentQuery<T>> DocumentQuery<T>::create(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::optional<std::string> index_name, std::optional<std::string> collection_name, bool is_group_by,
		std::shared_ptr<tokens::DeclareToken> declare_token, std::vector<std::shared_ptr<tokens::LoadToken>>  load_tokens,
		std::optional<std::string> from_alias)
	{
		auto new_object = std::shared_ptr<DocumentQuery<T>>(new DocumentQuery<T>(
			session, std::move(index_name), std::move(collection_name), is_group_by,
			declare_token, std::move(load_tokens), std::move(from_alias)));
		new_object->_weak_this = new_object;
		new_object->complete_construction();

		return new_object;
	}

	template <typename T>
	std::shared_ptr<conventions::DocumentConventions> DocumentQuery<T>::get_conventions() const
	{
		return AbstractDocumentQuery<T>::get_conventions();
	}

	template <typename T>
	std::vector<std::shared_ptr<T>> DocumentQuery<T>::to_list(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return AbstractDocumentQuery<T>::to_list(from_json);
	}

	template <typename T>
	Lazy<std::vector<std::shared_ptr<T>>> DocumentQuery<T>::lazily(
		const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval)
	{
		return AbstractDocumentQuery<T>::lazily(on_eval);
	}

	template <typename T>
	template <typename TProjection>
	std::shared_ptr<IDocumentQuery<TProjection, DocumentQuery<TProjection>>> DocumentQuery<T>::select_fields()
	{
		const auto& fields = impl::utils::GetTypeProjectionFields::get_projection_fields<TProjection>();

		return select_fields<TProjection>(queries::QueryData(fields, fields));
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::distinct()
	{
		AbstractDocumentQuery<T>::_distinct();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_score()
	{
		AbstractDocumentQuery<T>::_order_by_score();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_score_descending()
	{
		AbstractDocumentQuery<T>::_order_by_descending();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::include_explanation(
		std::shared_ptr<queries::explanation::Explanations>& explanations_reference)
	{
		AbstractDocumentQuery<T>::_include_explanations({}, explanations_reference);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::include_explanations(
		const std::optional<queries::explanation::ExplanationOptions>& options,
		std::shared_ptr<queries::explanation::Explanations>& explanations_reference)
	{
		AbstractDocumentQuery<T>::_include_explanations(options, explanations_reference);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::timings(std::shared_ptr<queries::timings::QueryTimings>& timings)
	{
		AbstractDocumentQuery<T>::_include_timings(timings);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TProjection>
	std::shared_ptr<IDocumentQuery<TProjection, DocumentQuery<TProjection>>> DocumentQuery<T>::select_fields(const std::vector<std::string>& fields)
	{
		auto query_data = queries::QueryData(fields, fields);
		return select_fields<TProjection>(query_data);
	}

	template <typename T>
	template <typename TProjection>
	std::shared_ptr<IDocumentQuery<TProjection, DocumentQuery<TProjection>>> DocumentQuery<T>::select_fields(const queries::QueryData& query_data)
	{
		return create_document_query_internal<TProjection>(query_data);
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::wait_for_non_stale_results(
		const std::optional<std::chrono::milliseconds>& wait_timeout)
	{
		AbstractDocumentQuery<T>::_wait_for_non_stale_results(std::move(wait_timeout));
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	queries::IndexQuery DocumentQuery<T>::get_index_query()
	{
		return AbstractDocumentQuery<T>::get_index_query();
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::add_parameter(std::string name, nlohmann::json object)
	{
		AbstractDocumentQuery<T>::_add_parameter(std::move(name), std::move(object));
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::add_order(const std::string& field_name, bool descending,
		OrderingType ordering)
	{
		if(descending)
		{
			order_by_descending(field_name, ordering);
		}
		else
		{
			order_by(field_name, ordering);
		}
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::add_after_query_executed_listener(
		std::function<void(const queries::QueryResult&)> action)
	{
		AbstractDocumentQuery<T>::_add_after_query_executed_listener(action);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::remove_after_query_executed_listener(
		std::function<void(const queries::QueryResult&)> action)
	{
		AbstractDocumentQuery<T>::_remove_after_query_executed_listener(action);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::add_after_stream_executed_listener(
		std::function<void(const nlohmann::json&)> action)
	{
		AbstractDocumentQuery<T>::_add_after_stream_executed_listener(action);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::remove_after_stream_executed_listener(
		std::function<void(const nlohmann::json&)> action)
	{
		AbstractDocumentQuery<T>::_remove_after_stream_executed_listener(action);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::open_subclause()
	{
		AbstractDocumentQuery<T>::_open_subclause();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::close_subclause()
	{
		AbstractDocumentQuery<T>::_close_subclause();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::search(const std::string& field_name, std::string search_terms,
		queries::SearchOperator search_operator)
	{
		AbstractDocumentQuery<T>::_search(field_name, std::move(search_terms), search_operator);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::intersect()
	{
		AbstractDocumentQuery<T>::_intersect();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::contains_any(const std::string& field_name,
		const std::vector<TValue>& values)
	{
		AbstractDocumentQuery<T>::template _contains_any<TValue>(field_name, values);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::contains_all(const std::string& field_name,
		const std::vector<TValue>& values)
	{
		AbstractDocumentQuery<T>::template _contains_all<TValue>(field_name, values);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::statistics(std::shared_ptr<QueryStatistics>& stats) const
	{
		AbstractDocumentQuery<T>::_statistics(stats);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::using_default_operator(queries::QueryOperator query_operator)
	{
		AbstractDocumentQuery<T>::_using_default_operator(query_operator);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::no_tracking()
	{
		AbstractDocumentQuery<T>::_no_tracking();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::no_caching()
	{
		AbstractDocumentQuery<T>::_no_caching();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::include(std::string path)
	{
		AbstractDocumentQuery<T>::_include(std::move(path));
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::include(
		const std::function<void(std::shared_ptr<loaders::IQueryIncludeBuilder<loaders::QueryIncludeBuilder>>)>&
		includes)
	{
		auto include_builder = loaders::QueryIncludeBuilder::create(get_conventions());
		includes(include_builder);
		AbstractDocumentQuery<T>::_include(include_builder);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::not_next()
	{
		AbstractDocumentQuery<T>::negate_next();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::take(int32_t count)
	{
		AbstractDocumentQuery<T>::_take(count);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::skip(int32_t count)
	{
		AbstractDocumentQuery<T>::_skip(count);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_lucene(const std::string& field_name,
		const std::string& where_clause, bool exact)
	{
		AbstractDocumentQuery<T>::_where_lucene(field_name, where_clause, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_equals(std::string field_name, const TValue& value,
		bool exact)
	{
		AbstractDocumentQuery<T>::template _where_equals<TValue>(std::move(field_name), &value, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_equals(std::string field_name, const MethodCall& method,
		bool exact)
	{
		AbstractDocumentQuery<T>::template _where_equals(std::move(field_name), &method, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_equals(WhereParams<TValue> where_params)
	{
		AbstractDocumentQuery<T>::template _where_equals<TValue>(std::move(where_params));
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_not_equals(std::string field_name, const TValue& value,
		bool exact)
	{
		AbstractDocumentQuery<T>::template _where_not_equals<TValue>(field_name, &value, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_not_equals(std::string field_name,
		const MethodCall& method, bool exact)
	{
		AbstractDocumentQuery<T>::_where_not_equals(field_name, &method, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_not_equals(WhereParams<TValue> where_params)
	{
		AbstractDocumentQuery<T>::template _where_not_equals<TValue>(std::move(where_params));
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_in(const std::string& field_name,
		const std::vector<TValue>& values, bool exact)
	{
		AbstractDocumentQuery<T>::template _where_in<TValue>(field_name, values, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_starts_with(std::string field_name, const TValue& value)
	{
		AbstractDocumentQuery<T>::template _where_starts_with<TValue>(field_name, &value);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_ends_with(std::string field_name, const TValue& value)
	{
		AbstractDocumentQuery<T>::template _where_ends_with<TValue>(field_name, &value);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_between(const std::string& field_name,
		const TValue& start, const TValue& end, bool exact)
	{
		AbstractDocumentQuery<T>::template _where_between<TValue>(field_name, &start, &end, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_greater_than(const std::string& field_name,
		const TValue& value, bool exact)
	{
		AbstractDocumentQuery<T>::template _where_greater_than<TValue>(field_name, &value, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_greater_than_or_equal(const std::string& field_name,
		const TValue& value, bool exact)
	{
		AbstractDocumentQuery<T>::template _where_greater_than_or_equal<TValue>(field_name, &value, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_less_than(const std::string& field_name,
		const TValue& value, bool exact)
	{
		AbstractDocumentQuery<T>::template _where_less_than<TValue>(field_name, &value, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TValue>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_less_than_or_equal(const std::string& field_name,
		const TValue& value, bool exact)
	{
		AbstractDocumentQuery<T>::template _where_less_than_or_equal<TValue>(field_name, &value, exact);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_exists(const std::string& field_name)
	{
		AbstractDocumentQuery<T>::_where_exists(field_name);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::where_regex(const std::string& field_name, std::string pattern)
	{
		AbstractDocumentQuery<T>::_where_regex(field_name, std::move(pattern));
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::and_also()
	{
		AbstractDocumentQuery<T>::_and_also();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::or_else()
	{
		AbstractDocumentQuery<T>::_or_else();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::boost(double boost)
	{
		AbstractDocumentQuery<T>::_boost(boost);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::fuzzy(double fuzzy)
	{
		AbstractDocumentQuery<T>::_fuzzy(fuzzy);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::proximity(int32_t proximity)
	{
		AbstractDocumentQuery<T>::_proximity(proximity);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::random_ordering()
	{
		AbstractDocumentQuery<T>::_random_ordering();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::random_ordering(const std::string& seed)
	{
		AbstractDocumentQuery<T>::_random_ordering(seed);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	template <typename TResult>
	std::shared_ptr<DocumentQuery<TResult>> DocumentQuery<T>::of_type()
	{
		return create_document_query_internal<TResult>();
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by(const std::string& field, OrderingType ordering)
	{
		AbstractDocumentQuery<T>::_order_by(field, ordering);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_descending(const std::string& field,
		OrderingType ordering)
	{
		AbstractDocumentQuery<T>::_order_by_descending(field, ordering);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::add_before_query_executed_listener(
		std::function<void(const queries::IndexQuery&)> action)
	{
		AbstractDocumentQuery<T>::_add_before_query_executed_listener(action);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::remove_before_query_executed_listener(
		std::function<void(const queries::IndexQuery&)> action)
	{
		AbstractDocumentQuery<T>::_remove_before_query_executed_listener(action);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	Lazy<int32_t> DocumentQuery<T>::count_lazily()
	{
		return AbstractDocumentQuery<T>::count_lazily();
	}

	template <typename T>
	std::shared_ptr<T> DocumentQuery<T>::first(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return AbstractDocumentQuery<T>::first(from_json);
	}

	template <typename T>
	std::shared_ptr<T> DocumentQuery<T>::first_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return AbstractDocumentQuery<T>::first_or_default(from_json);
	}

	template <typename T>
	std::shared_ptr<T> DocumentQuery<T>::single(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return AbstractDocumentQuery<T>::single(from_json);
	}

	template <typename T>
	std::shared_ptr<T> DocumentQuery<T>::single_or_default(
		const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return AbstractDocumentQuery<T>::single_or_default(from_json);
	}

	template <typename T>
	bool DocumentQuery<T>::any()
	{
		return AbstractDocumentQuery<T>::any();
	}

	template <typename T>
	int32_t DocumentQuery<T>::count()
	{
		return AbstractDocumentQuery<T>::count();
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::negate_next()
	{
		AbstractDocumentQuery<T>::negate_next();
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::within_radius_of(
		const std::string& field_name, double radius, double latitude, double longitude,
		const std::optional<indexes::spatial::SpatialUnits>& radius_units, double dist_error_percent)
	{
		AbstractDocumentQuery<T>::_within_radius_of(field_name, radius, latitude, longitude, radius_units, dist_error_percent);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::relates_to_shape(
		const std::string& field_name, const std::string& shape_wkt, indexes::spatial::SpatialRelation relation,
		const std::optional<indexes::spatial::SpatialUnits>& units, double dist_error_percent)
	{
		AbstractDocumentQuery<T>::_spatial(field_name, shape_wkt, relation, units, dist_error_percent);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::spatial(const std::string& field_name,
		std::function<std::unique_ptr<queries::spatial::SpatialCriteria>(const queries::spatial::SpatialCriteriaFactory&)
		> clause)
	{
		auto criteria = clause(queries::spatial::SpatialCriteriaFactory::INSTANCE);
		AbstractDocumentQuery<T>::_spatial(field_name, *criteria);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::spatial(
		const queries::spatial::DynamicSpatialField& field_name,
		std::function<std::unique_ptr<queries::spatial::SpatialCriteria>(const queries::spatial::SpatialCriteriaFactory&
		)> clause)
	{
		auto criteria = clause(queries::spatial::SpatialCriteriaFactory::INSTANCE);
		AbstractDocumentQuery<T>::_spatial(field_name, *criteria);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::more_like_this(
		const queries::more_like_this::MoreLikeThisBase& more_like_this)
	{
		auto mlt = AbstractDocumentQuery<T>::_more_like_this();
		mlt.with_options(more_like_this.options);

		if(auto mlt_ud = dynamic_cast<const queries::more_like_this::MoreLikeThisUsingDocument*>(&more_like_this))
		{
			mlt.with_document(mlt_ud->document_json);
		}

		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::highlight(std::string field_name,
		int32_t fragment_length, int32_t fragment_count,
		std::shared_ptr<queries::highlighting::Highlightings>& highlightings_reference)
	{
		AbstractDocumentQuery<T>::_highlight(std::move(field_name), fragment_length, fragment_count, {}, highlightings_reference);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::highlight(std::string field_name,
		int32_t fragment_length, int32_t fragment_count,
		const std::optional<queries::highlighting::HighlightingOptions>& options,
		std::shared_ptr<queries::highlighting::Highlightings>& highlightings_reference)
	{
		AbstractDocumentQuery<T>::_highlight(std::move(field_name), fragment_length, fragment_count, options, highlightings_reference);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_distance(
		const queries::spatial::DynamicSpatialField& field, double latitude, double longitude)
	{
		AbstractDocumentQuery<T>::_order_by_distance(field, latitude, longitude);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_distance(
		const std::string& field_name, double latitude, double longitude)
	{
		AbstractDocumentQuery<T>::_order_by_distance(field_name, latitude, longitude);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_distance(
		const queries::spatial::DynamicSpatialField& field, const std::string& shape_wkt)
	{
		AbstractDocumentQuery<T>::_order_by_distance(field, shape_wkt);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_distance(
		const std::string& field_name, const std::string& shape_wkt)
	{
		AbstractDocumentQuery<T>::_order_by_distance(field_name, shape_wkt);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_distance_descending(
		const queries::spatial::DynamicSpatialField& field, double latitude, double longitude)
	{
		AbstractDocumentQuery<T>::_order_by_distance_descending(field, latitude, longitude);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_distance_descending(
		const std::string& field_name, double latitude, double longitude)
	{
		AbstractDocumentQuery<T>::_order_by_distance_descending(field_name, latitude, longitude);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_distance_descending(
		const queries::spatial::DynamicSpatialField& field, const std::string& shape_wkt)
	{
		AbstractDocumentQuery<T>::_order_by_distance_descending(field, shape_wkt);
		return std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock());
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentQuery<T>::order_by_distance_descending(
		const std::string& field_name, const std::string& shape_wkt)
	{
		AbstractDocumentQuery<T>::_order_by_distance_descending(field_name, shape_wkt);
		return std::static_pointer_cast<IDocumentQuery<T, DocumentQuery<T>>>(
			std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock()));
	}

	template <typename T>
	const std::optional<std::string>& DocumentQuery<T>::get_index_name() const
	{
		return AbstractDocumentQuery<T>::get_index_name();
	}

	template <typename T>
	bool DocumentQuery<T>::is_distinct() const
	{
		return AbstractDocumentQuery<T>::is_distinct();
	}

	template <typename T>
	queries::QueryResult DocumentQuery<T>::get_query_result()
	{
		return AbstractDocumentQuery<T>::get_query_result();
	}

	template <typename T>
	std::shared_ptr<IGroupByDocumentQuery<T, GroupByDocumentQuery<T>>> DocumentQuery<T>::group_by(
		const std::vector<std::string>& field_names)
	{
		AbstractDocumentQuery<T>::_group_by(field_names);
		return GroupByDocumentQuery<T>::create(
			std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock()));
	}

	template <typename T>
	std::shared_ptr<IGroupByDocumentQuery<T, GroupByDocumentQuery<T>>> DocumentQuery<T>::group_by(
		const std::vector<queries::GroupBy>& fields)
	{
		AbstractDocumentQuery<T>::_group_by(fields);
		return GroupByDocumentQuery<T>::create(
			std::static_pointer_cast<DocumentQuery<T>>(AbstractDocumentQuery<T>::_weak_this.lock()));
	}
}
