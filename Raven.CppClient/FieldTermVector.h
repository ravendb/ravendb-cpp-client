#pragma once
#include "json.hpp"

namespace ravendb::client::documents::indexes
{
	enum class FieldTermVector
	{
		NO,
		YES,
		WITH_POSITIONS,
		WITH_OFFSETS,
		WITH_POSITIONS_AND_OFFSETS,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(FieldTermVector,
	{
		{FieldTermVector::UNSET, nullptr},
		{FieldTermVector::NO, "No"},
		{FieldTermVector::YES, "Yes"},
		{FieldTermVector::WITH_POSITIONS, "WithPositions"},
		{FieldTermVector::WITH_OFFSETS, "WithOffsets"},
		{FieldTermVector::WITH_POSITIONS_AND_OFFSETS, "WithPositionsAndOffsets"}
	});
}
