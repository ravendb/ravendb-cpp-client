#pragma once

namespace ravendb::client::documents::indexes
{
	enum class IndexType
	{
		NONE,
		AUTO_MAP,
		AUTO_MAP_REDUCE,
		MAP,
		MAP_REDUCE,
		FAULTY,
		JAVASCRIPT_MAP,
		JAVASCRIPT_MAP_REDUCE,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(IndexType,
	{
		{IndexType::UNSET, nullptr},
		{IndexType::NONE, "None"},
		{IndexType::AUTO_MAP, "AutoMap"},
		{IndexType::AUTO_MAP_REDUCE, "AutoMapReduce"},
		{IndexType::MAP, "Map"},
		{IndexType::MAP_REDUCE, "MapReduce"},
		{IndexType::FAULTY, "Faulty"},
		{IndexType::JAVASCRIPT_MAP, "JavaScriptMap"},
		{IndexType::JAVASCRIPT_MAP_REDUCE, "JavaScriptMapReduce"}
	});
}
