#pragma once

namespace ravendb::client::documents::indexes::spacial
{
	enum class SpatialSearchStrategy
	{
		GEOHASH_PREFIX_TREE,
		QUAD_PREFIX_TREE,
		BOUNDING_BOX,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(SpatialSearchStrategy,
	{
		{SpatialSearchStrategy::UNSET, nullptr},
		{SpatialSearchStrategy::GEOHASH_PREFIX_TREE, "GeohashPrefixTree"},
		{SpatialSearchStrategy::QUAD_PREFIX_TREE, "QuadPrefixTree"},
		{SpatialSearchStrategy::BOUNDING_BOX, "BoundingBox"}
	});
}
