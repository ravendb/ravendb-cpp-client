#pragma once
#include "json.hpp"

namespace ravendb::client::documents::queries::suggestions
{
	enum class SuggestionSortMode
	{
		NONE,
		POPULARITY
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(SuggestionSortMode,
		{
			{SuggestionSortMode::NONE, "None"},
			{SuggestionSortMode::POPULARITY, "Popularity"}
		})
}