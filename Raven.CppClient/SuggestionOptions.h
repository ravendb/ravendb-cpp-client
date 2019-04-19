#pragma once
#include "StringDistanceTypes.h"
#include "SuggestionSortMode.h"

namespace ravendb::client::documents::queries::suggestions
{
	struct SuggestionOptions
	{
		static const SuggestionOptions default_options;

		static const float DEFAULT_ACCURACY;

		static const int32_t DEFAULT_PAGE_SIZE;

		static const StringDistanceTypes DEFAULT_DISTANCE;

		static const SuggestionSortMode DEFAULT_SORT_MODE;

		int32_t page_size = DEFAULT_PAGE_SIZE;
		StringDistanceTypes distance = DEFAULT_DISTANCE;
		std::optional<float> accuracy = DEFAULT_ACCURACY;
		SuggestionSortMode sort_mode = DEFAULT_SORT_MODE;


		friend bool operator==(const SuggestionOptions& lhs, const SuggestionOptions& rhs);
		friend bool operator!=(const SuggestionOptions& lhs, const SuggestionOptions& rhs);
	};

	void to_json(nlohmann::json& j, const SuggestionOptions& so);

}
