#include "stdafx.h"
#include "SuggestionWithTerm.h"

namespace ravendb::client::documents::queries::suggestions
{
	SuggestionWithTerm::~SuggestionWithTerm() = default;

	SuggestionWithTerm::SuggestionWithTerm(std::string field_param)
		: SuggestionBase(std::move(field_param))
	{}
}
