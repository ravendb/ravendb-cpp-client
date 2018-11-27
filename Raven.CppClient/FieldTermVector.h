#pragma once

namespace ravendb::client::documents::indexes
{
	enum class FieldTermVector
	{
		No,
		Yes,
		WithPositions,
		WithOffsets,
		WithPositionsAndOffsets
	};
}
