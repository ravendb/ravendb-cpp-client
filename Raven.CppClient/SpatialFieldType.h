#pragma once
#include "nlohmann/json.hpp"


namespace ravendb::client::documents::indexes::spatial
{
	enum class SpatialFieldType
	{
		GEOGRAPHY,
		CARTESIAN,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(SpatialFieldType,
	{
		{SpatialFieldType::UNSET, nullptr},
		{SpatialFieldType::GEOGRAPHY, "Geography"},
		{SpatialFieldType::CARTESIAN, "Cartesian"}
	})
}
