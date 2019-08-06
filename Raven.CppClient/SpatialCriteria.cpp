#include "stdafx.h"
#include "SpatialCriteria.h"
#include "WhereOperator.h"
#include "WhereToken.h"

namespace ravendb::client::documents::queries::spatial
{
	SpatialCriteria::SpatialCriteria(indexes::spatial::SpatialRelation relation, double distance_error_pct)
		: _relation(relation)
		, _distance_error_pct(distance_error_pct)
	{}

	SpatialCriteria::~SpatialCriteria() = default;

	std::shared_ptr<session::tokens::QueryToken> SpatialCriteria::to_query_token(std::string field_name,
		std::function<std::string(nlohmann::json)> add_query_parameter) const
	{
		auto shape_token = get_shape_token(add_query_parameter);

		session::tokens::WhereOperator where_operator{};

		switch (_relation)
		{
		case indexes::spatial::SpatialRelation::WITHIN:
			where_operator = session::tokens::WhereOperator::SPATIAL_WITHIN;
			break;
		case indexes::spatial::SpatialRelation::CONTAINS:
			where_operator = session::tokens::WhereOperator::SPATIAL_CONTAINS;
			break;
		case indexes::spatial::SpatialRelation::DISJOINT:
			where_operator = session::tokens::WhereOperator::SPATIAL_DISJOINT;
			break;
		case indexes::spatial::SpatialRelation::INTERSECTS:
			where_operator = session::tokens::WhereOperator::SPATIAL_INTERSECTS;
			break;
		default:
			throw std::invalid_argument("'_relation' is of unknown type");
		}

		return session::tokens::WhereToken::create(where_operator, std::move(field_name), {},
			session::tokens::WhereToken::WhereOptions(shape_token, _distance_error_pct));
	}
}
