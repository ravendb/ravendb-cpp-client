#include "stdafx.h"
#include "IndexTypeExtensions.h"

namespace ravendb::client::documents::indexes
{
	bool IndexTypeExtensions::is_map(IndexType type)
	{
		return
			type == IndexType::MAP ||
			type == IndexType::AUTO_MAP ||
			type == IndexType::JAVASCRIPT_MAP;
	}

	bool IndexTypeExtensions::is_map_reduce(IndexType type)
	{
		return
			type == IndexType::MAP_REDUCE ||
			type == IndexType::AUTO_MAP_REDUCE ||
			type == IndexType::JAVASCRIPT_MAP_REDUCE;
	}

	bool IndexTypeExtensions::is_auto(IndexType type)
	{
		return
			type == IndexType::AUTO_MAP ||
			type == IndexType::AUTO_MAP_REDUCE;
	}

	bool IndexTypeExtensions::is_state(IndexType type)
	{
		return
			type == IndexType::MAP ||
			type == IndexType::MAP_REDUCE ||
			type == IndexType::JAVASCRIPT_MAP ||
			type == IndexType::JAVASCRIPT_MAP_REDUCE;
	}

	bool IndexTypeExtensions::is_java_script(IndexType type)
	{
		return
			type == IndexType::JAVASCRIPT_MAP ||
			type == IndexType::JAVASCRIPT_MAP_REDUCE;
	}
}
