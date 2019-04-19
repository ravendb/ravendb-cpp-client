#include "stdafx.h"
#include "SuggestionWithTerms.h"

namespace ravendb::client::documents::queries::suggestions
{
	SuggestionWithTerms::~SuggestionWithTerms() = default;

	SuggestionWithTerms::SuggestionWithTerms(std::string field_param)
		: SuggestionBase(std::move(field_param))
	{}
}
