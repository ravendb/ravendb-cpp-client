#include "stdafx.h"
#include "SuggestionOptions.h"
#include "json_utils.h"

namespace ravendb::client::documents::queries::suggestions
{
	const SuggestionOptions SuggestionOptions::default_options{};

	const float SuggestionOptions::DEFAULT_ACCURACY = 0.5f;

	const int32_t SuggestionOptions::DEFAULT_PAGE_SIZE = 15;

	const StringDistanceTypes SuggestionOptions::DEFAULT_DISTANCE = StringDistanceTypes::LEVENSHTEIN;

	const SuggestionSortMode SuggestionOptions::DEFAULT_SORT_MODE = SuggestionSortMode::POPULARITY;

	bool operator==(const SuggestionOptions& lhs, const SuggestionOptions& rhs)
	{
		return lhs.page_size == rhs.page_size
			&& lhs.distance == rhs.distance
			&& lhs.accuracy == rhs.accuracy
			&& lhs.sort_mode == rhs.sort_mode;
	}

	bool operator!=(const SuggestionOptions& lhs, const SuggestionOptions& rhs)
	{
		return !(lhs == rhs);
	}

	void to_json(nlohmann::json& j, const SuggestionOptions& so)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "PageSize", so.page_size);
		set_val_to_json(j, "Distance", so.distance);
		set_val_to_json(j, "Accuracy", so.accuracy);
		set_val_to_json(j, "SortMode", so.sort_mode);
	}
}
