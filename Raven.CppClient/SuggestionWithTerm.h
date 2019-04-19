#pragma once
#include "SuggestionBase.h"

namespace ravendb::client::documents::queries::suggestions
{
	struct SuggestionWithTerm : SuggestionBase
	{
		std::string term{};

		~SuggestionWithTerm() override;
		explicit SuggestionWithTerm(std::string field_param);
	};
}
