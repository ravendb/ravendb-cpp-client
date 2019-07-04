#pragma once
#include "IDocumentQueryBaseSingle.h"
#include "IDocumentQueryBase.h"
#include "IEnumerableQuery.h"
#include "GroupBy.h"
#include "QueryData.h"

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IDocumentQuery :
		public IDocumentQueryBase<T, TThis>,
		public IDocumentQueryBaseSingle<T, TThis>,
		public IEnumerableQuery<T, TThis>
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IDocumentQuery, TThis>,
				"'TThis' should be derived from IDocumentQuery");
			return static_cast<TThis*>(this);
		}

	protected:
		IDocumentQuery() = default;

	public:
		const std::optional<std::string>& get_index_name() const;

		bool is_distinct() const;

		queries::QueryResult get_query_result();

		template<typename TProjection>
		auto select_fields();

		template<typename TProjection>
		auto select_fields(const std::vector<std::string>& fields);

		template<typename TProjection>
		auto select_fields(const queries::QueryData& query_data);

		template<typename TResult>
		auto of_type();

		auto group_by(const std::vector<std::string>& field_names);

		auto group_by(const std::vector<queries::GroupBy>& fields);

		//TODO
		//IDocumentQuery<T> moreLikeThis(Consumer<IMoreLikeThisBuilderForDocumentQuery<T>> builder);

		//IAggregationDocumentQuery<T> aggregateBy(Consumer<IFacetBuilder<T>> builder);

		//IAggregationDocumentQuery<T> aggregateBy(FacetBase facet);

		//IAggregationDocumentQuery<T> aggregateBy(Facet... facet);

		//IAggregationDocumentQuery<T> aggregateUsing(String facetSetupDocumentId);

		//ISuggestionDocumentQuery<T> suggestUsing(SuggestionBase suggestion);

		//ISuggestionDocumentQuery<T> suggestUsing(Consumer<ISuggestionBuilder<T>> builder);

	};

	template <typename T, class TThis>
	const std::optional<std::string>& IDocumentQuery<T, TThis>::get_index_name() const
	{
		return cast_down()->get_index_name();
	}

	template <typename T, class TThis>
	bool IDocumentQuery<T, TThis>::is_distinct() const
	{
		return cast_down()->is_distinct();
	}

	template <typename T, class TThis>
	queries::QueryResult IDocumentQuery<T, TThis>::get_query_result()
	{
		return cast_down()->get_query_result();
	}

	template <typename T, class TThis>
	template <typename TProjection>
	auto IDocumentQuery<T, TThis>::select_fields()
	{
		return cast_down()->template select_fields<TProjection>();
	}

	template <typename T, class TThis>
	template <typename TProjection>
	auto IDocumentQuery<T, TThis>::select_fields(const std::vector<std::string>& fields)
	{
		return cast_down()->template select_fields<TProjection>(fields);
	}

	template <typename T, class TThis>
	template <typename TProjection>
	auto IDocumentQuery<T, TThis>::select_fields(const queries::QueryData& query_data)
	{
		return cast_down()->template select_fields<TProjection>(query_data);
	}

	template <typename T, class TThis>
	template <typename TResult>
	auto IDocumentQuery<T, TThis>::of_type()
	{
		return cast_down()->template of_type<TResult>();
	}

	template <typename T, class TThis>
	auto IDocumentQuery<T, TThis>::group_by(const std::vector<std::string>& field_names)
	{
		return cast_down()->group_by(field_names);
	}

	template <typename T, class TThis>
	auto IDocumentQuery<T, TThis>::group_by(const std::vector<queries::GroupBy>& fields)
	{
		return cast_down()->group_by(fields);
	}
}
