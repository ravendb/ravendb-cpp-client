#pragma once
#include "SpatialRelation.h"
#include "ShapeToken.h"

namespace ravendb::client::documents::queries::spatial
{
	class SpatialCriteria
	{
	private:
		const indexes::spatial::SpatialRelation _relation;
		const double _distance_error_pct;

	protected:
		SpatialCriteria(indexes::spatial::SpatialRelation relation, double distance_error_pct);

		virtual std::shared_ptr<session::tokens::ShapeToken> get_shape_token(
			std::function<std::string(nlohmann::json)> add_query_parameter) const = 0;

	public:
		virtual ~SpatialCriteria() = 0;

		std::shared_ptr<session::tokens::QueryToken> to_query_token(std::string field_name,
			std::function<std::string(nlohmann::json)> add_query_parameter) const;
	};
}
