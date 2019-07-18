#pragma once
#include "json.hpp"

namespace ravendb::client::documents::indexes::spatial
{
	enum class SpatialUnits
	{
		KILOMETERS,
		MILES,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(SpatialUnits,
	{
		{SpatialUnits::UNSET, nullptr},
		{SpatialUnits::KILOMETERS, "Kilometers"},
		{SpatialUnits::MILES, "Miles"}
	});
}
