#pragma once
#include "SpatialUnits.h"
#include "SpatialCriteria.h"

namespace ravendb::client::documents::queries::spatial
{
	class CircleCriteria : public SpatialCriteria
	{
	private:
		const double _radius;
		const double _latitude;
		const double _longitude;
		const std::optional<indexes::spatial::SpatialUnits> _radius_units;

	protected:
		std::shared_ptr<session::tokens::ShapeToken> get_shape_token(
			std::function<std::string(nlohmann::json)> add_query_parameter) const override;

	public:
		~CircleCriteria() override = default;

		CircleCriteria(double radius, double latitude, double longitude,
			std::optional<indexes::spatial::SpatialUnits> radius_units,
			indexes::spatial::SpatialRelation relation, double distance_error_pct);
	};
}
