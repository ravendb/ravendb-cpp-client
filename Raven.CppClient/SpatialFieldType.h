#pragma once

namespace ravendb::client::documents::indexes::spacial
{
	enum class SpatialFieldType
	{
		GEOGRAPHY,
		CARTESIAN,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(FieldTermVector,
	{
		{SpatialFieldType::UNSET, nullptr},
		{SpatialFieldType::GEOGRAPHY, "Geography"},
		{SpatialFieldType::CARTESIAN, "Cartasian"}
	});
}
