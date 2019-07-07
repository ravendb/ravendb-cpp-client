#include "stdafx.h"
#include "CircleCriteria.h"

namespace ravendb::client::documents::queries::spatial
{
	std::shared_ptr<session::tokens::ShapeToken> CircleCriteria::get_shape_token(
		std::function<std::string(nlohmann::json)> add_query_parameter) const
	{
		auto&& radius_param = add_query_parameter(_radius);
		auto&& latitude_param = add_query_parameter(_latitude);
		auto&& longitude_param = add_query_parameter(_longitude);

		return session::tokens::ShapeToken::circle(
			radius_param,
			latitude_param,
			longitude_param,
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
		, _radius_units(radius_units)
	{}


}
