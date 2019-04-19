#pragma once
#include "SpatialOptions.h"

namespace ravendb::client::documents::indexes::spatial
{
	class SpatialOptionsFactory
	{
	public:
		class GeographySpatialOptionsFactory;
		class CartesianSpatialOptionsFactory;

		static GeographySpatialOptionsFactory geography();
		static CartesianSpatialOptionsFactory cartesian();

		class GeographySpatialOptionsFactory
		{
			SpatialOptions default_options() const;
			SpatialOptions default_options(SpatialUnits circle_radius_units) const;

			SpatialOptions bounding_box_index() const;
			SpatialOptions bounding_box_index(SpatialUnits circle_radius_units) const;

			SpatialOptions geohash_prefix_tree_index(int32_t max_tree_level) const;
			SpatialOptions geohash_prefix_tree_index(int32_t max_tree_level, SpatialUnits circle_radius_units) const;

			SpatialOptions quad_prefix_tree_index(int32_t max_tree_level) const;
			SpatialOptions quad_prefix_tree_index(int32_t max_tree_level, SpatialUnits circle_radius_units) const;
		};

		struct SpatialBounds;

		class CartesianSpatialOptionsFactory
		{
			SpatialOptions bounding_box_index() const;

			SpatialOptions quad_prefix_tree_index(int32_t max_tree_level, SpatialBounds bounds) const;
		};

		struct SpatialBounds
		{
			double minX{};
			double maxX{};
			double minY{};
			double maxY{};

			SpatialBounds(double minX_param, double minY_param, double maxX_param, double maxY_param);

			friend bool operator==(const SpatialBounds& lhs, const SpatialBounds& rhs)
			{
				return lhs.minX == rhs.minX
					&& lhs.maxX == rhs.maxX
					&& lhs.minY == rhs.minY
					&& lhs.maxY == rhs.maxY;
			}

			friend bool operator!=(const SpatialBounds& lhs, const SpatialBounds& rhs)
			{
				return !(lhs == rhs);
			}

			//TODO implement
			std::size_t hash_code() const;
		};
	};
}


