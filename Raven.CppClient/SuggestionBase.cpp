#include "stdafx.h"
#include "SuggestionBase.h"

namespace ravendb::client::documents::queries::suggestions
{
	SuggestionBase::~SuggestionBase() = default;

	SuggestionBase::SuggestionBase(std::string field_param)
		: field(std::move(field_param))
	{}
}
