#pragma once
#include "SuggestionBase.h"

namespace ravendb::client::documents::queries::suggestions
{
	struct SuggestionWithTerms : SuggestionBase
	{
		std::vector<std::string> terms{};

		~SuggestionWithTerms() override;
		explicit SuggestionWithTerms(std::string field_param);
	};
}