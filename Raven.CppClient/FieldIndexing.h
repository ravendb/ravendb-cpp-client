#pragma once

namespace ravendb::client::documents::indexes
{
	enum class FieldIndexing
	{
		NO,
		SEARCH,
		EXACT,
		DEFAULT,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(FieldIndexing,
	{
		{FieldIndexing::UNSET, nullptr},
		{FieldIndexing::NO, "No"},
		{FieldIndexing::SEARCH, "Search"},
		{FieldIndexing::EXACT, "Exact"},
		{FieldIndexing::DEFAULT, "Default"}
	});
}
