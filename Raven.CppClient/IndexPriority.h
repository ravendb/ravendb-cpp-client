#pragma once

namespace ravendb::client::documents::indexes
{
	enum class IndexPriority
	{
		LOW,
		NORMAL,
		HIGH,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(IndexPriority, 
	{
		{IndexPriority::UNSET, nullptr},
		{IndexPriority::LOW, "Low"},
		{IndexPriority::NORMAL, "Normal"},
		{IndexPriority::HIGH, "High"},
	});
}
