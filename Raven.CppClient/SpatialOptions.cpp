#include "stdafx.h"
#include "SpatialOptions.h"
#include "json_utils.h"

namespace ravendb::client::documents::indexes::spatial
{
	bool operator==(const SpatialOptions& lhs, const SpatialOptions& rhs)
	{
		return lhs.type == rhs.type
			&& lhs.strategy == rhs.strategy
			&& lhs.max_tree_level == rhs.max_tree_level
			&& lhs.minX == rhs.minX
			&& lhs.maxX == rhs.maxX
			&& lhs.minY == rhs.minY
			&& lhs.maxY == rhs.maxY
			&& lhs.units == rhs.units;
	}

	void to_json(nlohmann::json& j, const SpatialOptions& so)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Type", so.type);
		set_val_to_json(j, "Strategy", so.strategy);
		set_val_to_json(j, "MaxTreeLevel", so.max_tree_level);
		set_val_to_json(j, "MinX", so.minX);
		set_val_to_json(j, "MaxX", so.maxX);
		set_val_to_json(j, "MinY", so.minY);
		set_val_to_json(j, "MaxY", so.maxY);
		set_val_to_json(j, "Units", so.units);
	}

	void from_json(const nlohmann::json& j, SpatialOptions& so)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Type", so.type);
		get_val_from_json(j, "Strategy", so.strategy);
		get_val_from_json(j, "MaxTreeLevel", so.max_tree_level);
		get_val_from_json(j, "MinX", so.minX);
		get_val_from_json(j, "MaxX", so.maxX);
		get_val_from_json(j, "MinY", so.minY);
		get_val_from_json(j, "MaxY", so.maxY);
		get_val_from_json(j, "Units", so.units);
	}
}
