#pragma once
#include "IDocumentQueryBaseSingle.h"
#include "IDocumentQueryBase.h"
#include "QueryResult.h"

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IDocumentQuery : 
		public IDocumentQueryBase<T, TThis>,
		public IDocumentQueryBaseSingle<T, TThis>
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
		std::vector<std::shared_ptr<T>> to_list()
		{
			return cast_down()->to_list();
		}

		const std::string& get_index_name() const
		{
			return cast_down()->get_index_name();
		}

		bool is_distinct() const
		{
			return cast_down()->is_distinct();
		}

		queries::QueryResult get_query_result()
		{
			return cast_down()->get_query_result();
		}

		//TODO implement all overloads
		//<TProjection> IDocumentQuery<TProjection> selectFields(Class<TProjection> projectionClass);
		//<TResult> IDocumentQuery<TResult> ofType(Class<TResult> resultClass);

		//IGroupByDocumentQuery<T> groupBy(String fieldName, String... fieldNames);

		//IGroupByDocumentQuery<T> groupBy(GroupBy field, GroupBy... fields);

		//IDocumentQuery<T> moreLikeThis(Consumer<IMoreLikeThisBuilderForDocumentQuery<T>> builder);

		//IAggregationDocumentQuery<T> aggregateBy(Consumer<IFacetBuilder<T>> builder);

		//IAggregationDocumentQuery<T> aggregateBy(FacetBase facet);

		//IAggregationDocumentQuery<T> aggregateBy(Facet... facet);

		//IAggregationDocumentQuery<T> aggregateUsing(String facetSetupDocumentId);

		//ISuggestionDocumentQuery<T> suggestUsing(SuggestionBase suggestion);

		//ISuggestionDocumentQuery<T> suggestUsing(Consumer<ISuggestionBuilder<T>> builder);

	};

}
