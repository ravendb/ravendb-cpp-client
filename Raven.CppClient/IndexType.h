#pragma once

namespace ravendb::client::documents::indexes
{
	enum class IndexType
	{
		None,
		AutoMap,
		AutoMapReduce,
		Map,
		MapReduce,
		Faulty,
		JavaScriptMap,
		JavaScriptMapReduce,
	};
}
