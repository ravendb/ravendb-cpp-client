#pragma once

namespace ravendb::client::documents::indexes::spacial
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
