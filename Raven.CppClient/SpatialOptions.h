#pragma once
#include "SpatialFieldType.h"
#include "SpatialSearchStrategy.h"
#include "SpatialUnits.h"
#include "json.hpp"

namespace ravendb::client::documents::indexes::spatial
{
	struct SpatialOptions
	{
		//about 4.78 meters at equator, should be good enough (see: http://unterbahn.com/2009/11/metric-dimensions-of-geohash-partitions-at-the-equator/)
		static constexpr int32_t DEFAULT_GEOHASH_LEVEL = 9;

		//about 4.78 meters at equator, should be good enough
		static constexpr int32_t DEFAULT_QUAD_TREE_LEVEL = 23;

		SpatialFieldType type = SpatialFieldType::GEOGRAPHY;
		SpatialSearchStrategy strategy = SpatialSearchStrategy::GEOHASH_PREFIX_TREE;
		int32_t max_tree_level = DEFAULT_GEOHASH_LEVEL;
		double minX = -180;
		double maxX = 180;
		double minY = -90;
		double maxY = 90;
		// Circle radius units, only used for geography indexes
		SpatialUnits units = SpatialUnits::KILOMETERS;

		friend bool operator==(const SpatialOptions& lhs, const SpatialOptions& rhs);
	};

	void to_json(nlohmann::json& j, const SpatialOptions& so);

	void from_json(const nlohmann::json& j, SpatialOptions& so);
}
