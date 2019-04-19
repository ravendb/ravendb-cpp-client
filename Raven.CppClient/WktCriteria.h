#pragma once
#include "SpatialCriteria.h"

namespace ravendb::client::documents::queries::spatial
{
	class WktCriteria : public SpatialCriteria
	{
	private:
		const std::string _shape_wkt;
		const std::optional<indexes::spatial::SpatialUnits> _radius_units;

	protected:
		std::shared_ptr<session::tokens::ShapeToken> get_shape_token(std::function<std::string(nlohmann::json)> add_query_parameter) const override;

	public:
		~WktCriteria() override = default;

		WktCriteria(std::string shape_wtk, indexes::spatial::SpatialRelation relation,
			std::optional<indexes::spatial::SpatialUnits> radius_units,
			double distance_error_pct);
	};
}
