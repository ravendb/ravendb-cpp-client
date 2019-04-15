#include "stdafx.h"
#include "WktCriteria.h"

namespace ravendb::client::documents::queries::spatial
{
	std::shared_ptr<session::tokens::ShapeToken> WktCriteria::get_shape_token(
		std::function<std::string(nlohmann::json)> add_query_parameter) const
	{
		return session::tokens::ShapeToken::wkt(add_query_parameter(_shape_wkt), _radius_units);
	}

	WktCriteria::WktCriteria(std::string shape_wtk, 
		indexes::spatial::SpatialRelation relation,
		std::optional<indexes::spatial::SpatialUnits> radius_units,
		double distance_error_pct)
		: SpatialCriteria(relation, distance_error_pct)
		, _shape_wkt(std::move(shape_wtk))
		, _radius_units(std::move(radius_units))
	{}
}
