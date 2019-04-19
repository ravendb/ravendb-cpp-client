#pragma once
#include "SpatialCriteria.h"

namespace ravendb::client::documents::queries::spatial
{
	class SpatialCriteriaFactory
	{
	private:
		SpatialCriteriaFactory() = default;

	public:
		static const SpatialCriteriaFactory INSTANCE;

		std::unique_ptr<SpatialCriteria> relates_to_shape(std::string shape_wkt, indexes::spatial::SpatialRelation relation) const;

		std::unique_ptr<SpatialCriteria> relates_to_shape(std::string shape_wkt, indexes::spatial::SpatialRelation relation,
			double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> relates_to_shape(std::string shape_wkt, indexes::spatial::SpatialRelation relation,
			std::optional<indexes::spatial::SpatialUnits> units,
			double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> intersect(std::string shape_wkt) const;

		std::unique_ptr<SpatialCriteria> intersect(std::string shape_wkt, double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> intersect(std::string shape_wkt, std::optional<indexes::spatial::SpatialUnits> units) const;

		std::unique_ptr<SpatialCriteria> intersect(std::string shape_wkt, std::optional<indexes::spatial::SpatialUnits> units,
			double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> contains(std::string shape_wkt) const;

		std::unique_ptr<SpatialCriteria> contains(std::string shape_wkt, double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> contains(std::string shape_wkt, std::optional<indexes::spatial::SpatialUnits> units) const;

		std::unique_ptr<SpatialCriteria> contains(std::string shape_wkt, std::optional<indexes::spatial::SpatialUnits> units,
			double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> disjoint(std::string shape_wkt) const;

		std::unique_ptr<SpatialCriteria> disjoint(std::string shape_wkt, double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> disjoint(std::string shape_wkt, std::optional<indexes::spatial::SpatialUnits> units) const;

		std::unique_ptr<SpatialCriteria> disjoint(std::string shape_wkt, std::optional<indexes::spatial::SpatialUnits> units,
			double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> within(std::string shape_wkt) const;

		std::unique_ptr<SpatialCriteria> within(std::string shape_wkt, double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> within(std::string shape_wkt, std::optional<indexes::spatial::SpatialUnits> units) const;

		std::unique_ptr<SpatialCriteria> within(std::string shape_wkt, std::optional<indexes::spatial::SpatialUnits> units,
			double distance_error_pct) const;

		std::unique_ptr<SpatialCriteria> within_radius(double radius, double latitude, double longitude) const;

		std::unique_ptr<SpatialCriteria> within_radius(double radius, double latitude, double longitude,
			std::optional<indexes::spatial::SpatialUnits> radius_units) const;

		std::unique_ptr<SpatialCriteria> within_radius(double radius, double latitude, double longitude,
			std::optional<indexes::spatial::SpatialUnits> radius_units, double distance_error_pct) const;
	};
}
