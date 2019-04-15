#pragma once

namespace ravendb::client::documents::indexes::spatial
{
	enum class SpatialRelation
	{
		WITHIN,
		CONTAINS,
		DISJOINT,
		INTERSECTS,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(SpatialRelation,
		{
			{SpatialRelation::UNSET, nullptr},
			{SpatialRelation::WITHIN, "Within"},
			{SpatialRelation::CONTAINS, "Contains"},
			{SpatialRelation::DISJOINT, "Disjoint"},
			{SpatialRelation::INTERSECTS, "Intersects"}
		});
}
