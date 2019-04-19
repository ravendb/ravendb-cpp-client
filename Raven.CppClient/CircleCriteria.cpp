#include "stdafx.h"
#include "CircleCriteria.h"

namespace ravendb::client::documents::queries::spatial
{
	std::shared_ptr<session::tokens::ShapeToken> CircleCriteria::get_shape_token(
		std::function<std::string(nlohmann::json)> add_query_parameter) const
	{
		return session::tokens::ShapeToken::circle(add_query_parameter(_radius),
			add_query_parameter(_latitude),
			add_query_parameter(_longitude),
			_radius_units);
	}

	CircleCriteria::CircleCriteria(double radius, double latitude, double longitude,
		std::optional<indexes::spatial::SpatialUnits> radius_units,
		indexes::spatial::SpatialRelation relation,
		double distance_error_pct)
		: SpatialCriteria(relation, distance_error_pct)
		, _radius(radius)
		, _latitude(latitude)
		, _longitude(longitude)
		, _radius_units(std::move(radius_units))
	{}


}
