#include "stdafx.h"
#include "SpatialCriteriaFactory.h"
#include "Constants.h"
#include "WktCriteria.h"
#include "CircleCriteria.h"

namespace ravendb::client::documents::queries::spatial
{
	const SpatialCriteriaFactory SpatialCriteriaFactory::INSTANCE{};


	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::relates_to_shape(std::string shape_wkt,
		indexes::spatial::SpatialRelation relation) const
	{
		return relates_to_shape(std::move(shape_wkt), relation, constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::relates_to_shape(std::string shape_wkt,
		indexes::spatial::SpatialRelation relation, double distance_error_pct) const
	{
		return std::make_unique<WktCriteria>(std::move(shape_wkt), relation, std::optional<indexes::spatial::SpatialUnits>(),
			distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::relates_to_shape(std::string shape_wkt,
		indexes::spatial::SpatialRelation relation, std::optional<indexes::spatial::SpatialUnits> units,
		double distance_error_pct) const
	{
		return std::make_unique<WktCriteria>(std::move(shape_wkt), relation, std::move(units),
			distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::intersect(std::string shape_wkt) const
	{
		return intersect(std::move(shape_wkt), constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::intersect(std::string shape_wkt,
		double distance_error_pct) const
	{
		return relates_to_shape(std::move(shape_wkt), indexes::spatial::SpatialRelation::INTERSECTS, distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::intersect(std::string shape_wkt,
		std::optional<indexes::spatial::SpatialUnits> units) const
	{
		return intersect(std::move(shape_wkt), std::move(units),
			constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::intersect(std::string shape_wkt,
		std::optional<indexes::spatial::SpatialUnits> units, double distance_error_pct) const
	{
		return relates_to_shape(std::move(shape_wkt), indexes::spatial::SpatialRelation::INTERSECTS,
			std::move(units), distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::contains(std::string shape_wkt) const
	{
		return contains(std::move(shape_wkt), constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::contains(std::string shape_wkt,
		double distance_error_pct) const
	{
		return relates_to_shape(std::move(shape_wkt), indexes::spatial::SpatialRelation::CONTAINS, distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::contains(std::string shape_wkt,
		std::optional<indexes::spatial::SpatialUnits> units) const
	{
		return contains(std::move(shape_wkt), std::move(units), constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::contains(std::string shape_wkt,
		std::optional<indexes::spatial::SpatialUnits> units, double distance_error_pct) const
	{
		return relates_to_shape(std::move(shape_wkt), indexes::spatial::SpatialRelation::CONTAINS,
			std::move(units), distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::disjoint(std::string shape_wkt) const
	{
		return disjoint(std::move(shape_wkt), constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::disjoint(std::string shape_wkt,
		double distance_error_pct) const
	{
		return relates_to_shape(std::move(shape_wkt), indexes::spatial::SpatialRelation::DISJOINT, distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::disjoint(std::string shape_wkt,
		std::optional<indexes::spatial::SpatialUnits> units) const
	{
		return disjoint(std::move(shape_wkt), std::move(units), constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::disjoint(std::string shape_wkt,
		std::optional<indexes::spatial::SpatialUnits> units, double distance_error_pct) const
	{
		return relates_to_shape(std::move(shape_wkt), indexes::spatial::SpatialRelation::DISJOINT,
			std::move(units), distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::within(std::string shape_wkt) const
	{
		return within(std::move(shape_wkt), constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::within(std::string shape_wkt,
		double distance_error_pct) const
	{
		return relates_to_shape(std::move(shape_wkt), indexes::spatial::SpatialRelation::WITHIN, distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::within(std::string shape_wkt,
		std::optional<indexes::spatial::SpatialUnits> units) const
	{
		return within(std::move(shape_wkt), std::move(units), constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::within(std::string shape_wkt,
		std::optional<indexes::spatial::SpatialUnits> units, double distance_error_pct) const
	{
		return relates_to_shape(std::move(shape_wkt), indexes::spatial::SpatialRelation::WITHIN,
			std::move(units), distance_error_pct);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::within_radius(double radius, double latitude,
		double longitude) const
	{
		return within_radius(radius, latitude, longitude, {});
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::within_radius(double radius, double latitude,
		double longitude, std::optional<indexes::spatial::SpatialUnits> radius_units) const
	{
		return within_radius(radius, latitude, longitude, std::move(radius_units),
			constants::documents::indexing::spacial::DEFAULT_DISTANCE_ERROR_PCT);
	}

	std::unique_ptr<SpatialCriteria> SpatialCriteriaFactory::within_radius(double radius, double latitude,
		double longitude, std::optional<indexes::spatial::SpatialUnits> radius_units, double distance_error_pct) const
	{
		return std::make_unique<CircleCriteria>(radius, latitude, longitude, std::move(radius_units),
			indexes::spatial::SpatialRelation::WITHIN, distance_error_pct);
	}
}
