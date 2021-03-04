#pragma once
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::queries::suggestions
{
	enum class StringDistanceTypes
	{
		// Default, suggestion is not active
		NONE,

		//Default, equivalent to Levenshtein
		DEFAULT,

		//Levenshtein distance algorithm (default)
		LEVENSHTEIN,

		//JaroWinkler distance algorithm
		JARO_WINKLER,

		//NGram distance algorithm
		N_GRAM
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(StringDistanceTypes,
		{
			{StringDistanceTypes::NONE, "None"},
			{StringDistanceTypes::DEFAULT, "Default"},
			{StringDistanceTypes::LEVENSHTEIN, "Levenshtein"},
			{StringDistanceTypes::JARO_WINKLER, "JaroWinkler"},
			{StringDistanceTypes::N_GRAM, "NGram"}
		})
}
