#pragma once
#include "IQueryBase.h"
#include "SearchOperator.h"
#include "SpatialOptionsFactory.h"
#include "MoreLikeThisBase.h"
#include "SpatialCriteriaFactory.h"

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IFilterDocumentQueryBase :
		public IQueryBase<T, TThis>
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IFilterDocumentQueryBase, TThis>,
				"'TThis' should be derived from IFilterDocumentQueryBase");
			return static_cast<TThis*>(this);
		}

	protected:
		IFilterDocumentQueryBase() = default;

	public:
		auto not_next();

		auto and_also();

		auto close_subclause();

		template<typename TValue>
		auto contains_all(const std::string& field_name, const std::vector<TValue>& values);

		template<typename TValue>
		auto contains_any(const std::string& field_name, const std::vector<TValue>& values);

		auto negate_next();

		auto open_subclause();

		auto or_else();

		auto search(const std::string& field_name, std::string search_terms,
		                              queries::SearchOperator search_operator = queries::SearchOperator::OR);

		auto where_Lucene(const std::string& field_name, const std::string& where_clause,
		                                    bool exact = false);

		template<typename TValue>
		auto where_between(const std::string& field_name, const TValue& start, const TValue& end, bool exact = false);

		template<typename TValue>
		auto where_ends_with(std::string field_name, const TValue& value);

		template<typename TValue>
		auto where_equals(std::string field_name, const TValue& value, bool exact = false);

		auto where_equals(std::string field_name, const MethodCall& method, bool exact = false);

		template<typename TValue>
		auto where_equals(WhereParams<TValue> where_params);

		template<typename TValue>
		auto where_not_equals(std::string field_name, const TValue& value, bool exact = false);

		auto where_not_equals(std::string field_name, const MethodCall& method, bool exact = false);

		template<typename TValue>
		auto where_not_equals(WhereParams<TValue> where_params);

		template<typename TValue>
		auto where_greater_than(const std::string& field_name, const TValue& value, bool exact = false);

		template<typename TValue>
		auto where_greater_than_or_equal(const std::string& field_name, const TValue& value, bool exact = false);

		template<typename TValue>
		auto where_in(const std::string& field_name, const std::vector<TValue>& values, bool exact = false);

		template<typename TValue>
		auto where_less_than(const std::string& field_name, const TValue& value, bool exact = false);

		template<typename TValue>
		auto where_less_than_or_equal(const std::string& field_name, const TValue& value, bool exact = false);

		template<typename TValue>
		auto where_starts_with(std::string field_name, const TValue& value);

		auto where_exists(const std::string& field_name);

		auto where_regex(const std::string& field_name, std::string pattern);

		auto within_radius_of(const std::string& field_name, double radius, double latitude, double longitude,
			const std::optional<indexes::spatial::SpatialUnits>& radius_units = {},
			double dist_error_percent = constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);

		auto relates_to_shape(const std::string& field_name, const std::string& shape_wkt,
			indexes::spatial::SpatialRelation relation,
			const std::optional<indexes::spatial::SpatialUnits>& units = {},
			double dist_error_percent = constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);

		auto spatial(const std::string& field_name,
			std::function<std::unique_ptr<queries::spatial::SpatialCriteria>(const queries::spatial::SpatialCriteriaFactory&)> clause);
		
		auto spatial(const queries::spatial::DynamicSpatialField& field_name,
			std::function<std::unique_ptr<queries::spatial::SpatialCriteria>(const queries::spatial::SpatialCriteriaFactory&)> clause);

		auto more_like_this(const queries::more_like_this::MoreLikeThisBase& more_like_this);
	};

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::not_next()
	{
		return cast_down()->not_next();
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::and_also()
	{
		return cast_down()->and_also();
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::close_subclause()
	{
		return cast_down()->close_subclause();
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::contains_all(const std::string& field_name,
		const std::vector<TValue>& values)
	{
		return cast_down()->template contains_all<TValue>(field_name, values);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::contains_any(const std::string& field_name,
		const std::vector<TValue>& values)
	{
		return cast_down()->template contains_any<TValue>(field_name, values);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::negate_next()
	{
		return cast_down()->negate_next();
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::open_subclause()
	{
		return cast_down()->open_subclause();
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::or_else()
	{
		return cast_down()->or_else();
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::search(const std::string& field_name,
		std::string search_terms, queries::SearchOperator search_operator)
	{
		return cast_down()->search(field_name, search_terms, search_operator);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::where_Lucene(const std::string& field_name,
		const std::string& where_clause, bool exact)
	{
		return cast_down()->where_Lucene(field_name, where_clause, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_between(const std::string& field_name,
		const TValue& start, const TValue& end, bool exact)
	{
		return cast_down()->template where_between<TValue>(field_name, start, end, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_ends_with(std::string field_name,
		const TValue& value)
	{
		return cast_down()->template where_ends_with<TValue>(field_name, value);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_equals(std::string field_name,
		const TValue& value, bool exact)
	{
		return cast_down()->template where_equals<TValue>(std::move(field_name), value, exact);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::where_equals(std::string field_name,
		const MethodCall& method, bool exact)
	{
		return cast_down()->where_equals(std::move(field_name), method, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_equals(WhereParams<TValue> where_params)
	{
		return cast_down()->template where_equals<TValue>(std::move(where_params));
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_not_equals(std::string field_name,
		const TValue& value, bool exact)
	{
		return cast_down()->template where_not_equals<TValue>(std::move(field_name), value, exact);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::where_not_equals(std::string field_name,
		const MethodCall& method, bool exact)
	{
		return cast_down()->where_not_equals(std::move(field_name), method, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_not_equals(WhereParams<TValue> where_params)
	{
		return cast_down()->template where_not_equals<TValue>(std::move(where_params));
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_greater_than(const std::string& field_name,
		const TValue& value, bool exact)
	{
		return cast_down()->where_greater_than(field_name, value, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_greater_than_or_equal(
		const std::string& field_name, const TValue& value, bool exact)
	{
		return cast_down()->where_greater_than_or_equal(field_name, value, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_in(const std::string& field_name,
		const std::vector<TValue>& values, bool exact)
	{
		return cast_down()->template where_in<TValue>(field_name, values, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_less_than(const std::string& field_name,
		const TValue& value, bool exact)
	{
		return cast_down()->template where_less_than<TValue>(field_name, value, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_less_than_or_equal(const std::string& field_name,
		const TValue& value, bool exact)
	{
		return cast_down()->template where_less_than_or_equal<TValue>(field_name, value, exact);
	}

	template <typename T, class TThis>
	template <typename TValue>
	auto IFilterDocumentQueryBase<T, TThis>::where_starts_with(std::string field_name,
		const TValue& value)
	{
		return cast_down()->template where_starts_with<TValue>(field_name, value);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::where_exists(const std::string& field_name)
	{
		return cast_down()->where_exists(field_name);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::where_regex(const std::string& field_name,
		std::string pattern)
	{
		return cast_down()->where_regex(field_name, std::move(pattern));
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::within_radius_of(const std::string& field_name,
		double radius, double latitude, double longitude,
		const std::optional<indexes::spatial::SpatialUnits>& radius_units, double dist_error_percent)
	{
		return cast_down()->within_radius_of(field_name, radius, latitude, longitude, radius_units, dist_error_percent);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::relates_to_shape(const std::string& field_name,
		const std::string& shape_wkt, indexes::spatial::SpatialRelation relation,
		const std::optional<indexes::spatial::SpatialUnits>& units, double dist_error_percent)
	{
		return cast_down()->relates_to_shape(field_name, relation, units, dist_error_percent);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::spatial(const std::string& field_name,
		std::function<std::unique_ptr<queries::spatial::SpatialCriteria>(const queries::spatial::SpatialCriteriaFactory&)
		> clause)
	{
		return cast_down()->spatial(field_name, clause);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::spatial(
		const queries::spatial::DynamicSpatialField& field_name,
		std::function<std::unique_ptr<queries::spatial::SpatialCriteria>(const queries::spatial::SpatialCriteriaFactory&)
		> clause)
	{
		return cast_down()->spatial(field_name, clause);
	}

	template <typename T, class TThis>
	auto IFilterDocumentQueryBase<T, TThis>::more_like_this(
		const queries::more_like_this::MoreLikeThisBase& more_like_this)
	{
		return cast_down()->more_like_this(more_like_this);
	}
}
