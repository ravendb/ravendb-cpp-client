#pragma once
#include "SuggestionOptions.h"

namespace ravendb::client::documents::queries::suggestions
{
	struct SuggestionBase
	{
		std::string field{};
		std::optional<SuggestionOptions> options{};

		virtual ~SuggestionBase();

	protected:
		explicit SuggestionBase(std::string field_param);
	};
}
