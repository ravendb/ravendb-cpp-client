#pragma once
#include "IQueryBase.h"
#include "OrderingType.h"
#include "IFilterDocumentQueryBase.h"

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IDocumentQueryBase 
		: public IFilterDocumentQueryBase<T, TThis>
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IDocumentQueryBase, TThis>,
				"'TThis' should be derived from IDocumentQueryBase");
			return static_cast<TThis*>(this);
		}

	protected:
		IDocumentQueryBase() = default;

	public:
		auto add_order(const std::string& field_name, bool descending, OrderingType ordering = OrderingType::STRING);

		auto boost(double boost);

		auto distinct();

		auto include_explanation(
			std::optional<queries::explanation::Explanations>& explanations_reference);

		auto include_explanations(
			const std::optional<queries::explanation::ExplanationOptions>& options,
			std::optional<queries::explanation::Explanations>& explanations_reference);

		auto fuzzy(double fuzzy);

		auto highlight(std::string field_name, int32_t fragment_length, int32_t fragment_count,
			std::optional<queries::highlighting::Highlightings>& highlightings_reference);

		auto highlight(std::string field_name, int32_t fragment_length, int32_t fragment_count,
			const std::optional<queries::highlighting::HighlightingOptions>& options,
			std::optional<queries::highlighting::Highlightings>& highlightings_reference);

		auto include(std::string path);

		//TODO implement
		//auto include(const std::function<void(std::shared_ptr<IQueryIncludeBuilder>)>& includes)
		//{
		//	return cast_down()->include(includes);
		//}

		auto intersect();

		auto order_by(const std::string& field, OrderingType ordering = OrderingType::STRING);
		auto order_by_descending(const std::string& field, OrderingType ordering = OrderingType::STRING);

		auto order_by_score();
		auto order_by_score_descending();

		auto proximity(int32_t proximity);

		auto random_ordering();
		auto random_ordering(const std::string& seed);

		auto order_by_distance(const queries::spatial::DynamicSpatialField& field, double latitude, double longitude);
		auto order_by_distance(const std::string& field_name, double latitude, double longitude);
		auto order_by_distance(const queries::spatial::DynamicSpatialField& field, const std::string& shape_wkt);
		auto order_by_distance(const std::string& field_name, const std::string& shape_wkt);

		auto order_by_distance_descending(const queries::spatial::DynamicSpatialField& field, double latitude, double longitude);
		auto order_by_distance_descending(const std::string& field_name, double latitude, double longitude);
		auto order_by_distance_descending(const queries::spatial::DynamicSpatialField& field, const std::string& shape_wkt);
		auto order_by_distance_descending(const std::string& field_name, const std::string& shape_wkt);
	};

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::add_order(const std::string& field_name, bool descending,
		OrderingType ordering)
	{
		return cast_down()->add_order(field_name, descending, ordering);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::boost(double boost)
	{
		return cast_down()->boost(boost);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::distinct()
	{
		return cast_down()->distinct();
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::include_explanation(
		std::optional<queries::explanation::Explanations>& explanations_reference)
	{
		return cast_down()->include_explanation(explanations_reference);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::include_explanations(
		const std::optional<queries::explanation::ExplanationOptions>& options,
		std::optional<queries::explanation::Explanations>& explanations_reference)
	{
		return cast_down()->include_explanation(options, explanations_reference);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::fuzzy(double fuzzy)
	{
		return cast_down()->fuzzy(fuzzy);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::highlight(std::string field_name, int32_t fragment_length,
		int32_t fragment_count, std::optional<queries::highlighting::Highlightings>& highlightings_reference)
	{
		return cast_down()->highlight(std::move(field_name), fragment_length, fragment_count,
			highlightings_reference);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::highlight(std::string field_name, int32_t fragment_length,
		int32_t fragment_count, const std::optional<queries::highlighting::HighlightingOptions>& options,
		std::optional<queries::highlighting::Highlightings>& highlightings_reference)
	{
		return cast_down()->highlight(std::move(field_name), fragment_length, fragment_count,
			options, highlightings_reference);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::include(std::string path)
	{
		return cast_down()->include(std::move(path));
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::intersect()
	{
		return cast_down()->intersect();
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by(const std::string& field, OrderingType ordering)
	{
		return cast_down()->order_by(field, ordering);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_descending(const std::string& field, OrderingType ordering)
	{
		return cast_down()->order_by_descending(field, ordering);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_score()
	{
		return cast_down()->order_by_score();
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_score_descending()
	{
		return cast_down()->order_by_score_descending();
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::proximity(int32_t proximity)
	{
		return cast_down()->proximity(proximity);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::random_ordering()
	{
		return cast_down()->random_ordering();
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::random_ordering(const std::string& seed)
	{
		return cast_down()->random_ordering(seed);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_distance(
		const queries::spatial::DynamicSpatialField& field, double latitude, double longitude)
	{
		return cast_down()->order_by_distance(field, latitude, longitude);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_distance(const std::string& field_name,
		double latitude, double longitude)
	{
		return cast_down()->order_by_distance(field_name, latitude, longitude);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_distance(
		const queries::spatial::DynamicSpatialField& field, const std::string& shape_wkt)
	{
		return cast_down()->order_by_distance(field, shape_wkt);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_distance(const std::string& field_name,
		const std::string& shape_wkt)
	{
		return cast_down()->order_by_distance(field_name, shape_wkt);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_distance_descending(
		const queries::spatial::DynamicSpatialField& field, double latitude, double longitude)
	{
		return cast_down()->order_by_distance_descending(field, latitude, longitude);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_distance_descending(const std::string& field_name,
		double latitude, double longitude)
	{
		return cast_down()->order_by_distance_descending(field_name, latitude, longitude);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_distance_descending(
		const queries::spatial::DynamicSpatialField& field, const std::string& shape_wkt)
	{
		return cast_down()->order_by_distance_descending(field, shape_wkt);
	}

	template <typename T, class TThis>
	auto IDocumentQueryBase<T, TThis>::order_by_distance_descending(const std::string& field_name,
		const std::string& shape_wkt)
	{
		return cast_down()->order_by_distance_descending(field_name, shape_wkt);
	}
}
