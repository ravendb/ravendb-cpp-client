#include "stdafx.h"
#include "FacetToken.h"

namespace ravendb::client::documents::session::tokens
{
	FacetToken::FacetToken(std::optional<std::string> facet_setup_document_id)
		: _facet_setup_document_id(std::move(facet_setup_document_id))
	{}

	FacetToken::FacetToken(std::optional<std::string> aggregate_by_field_name, std::optional<std::string> alias,
		std::optional<std::vector<std::string>> ranges, std::optional<std::string> options_parameter_name)
		: _aggregate_by_field_name(std::move(aggregate_by_field_name))
		, _alias(std::move(alias))
		, _ranges(std::move(ranges))
		, _options_parameter_name(std::move(options_parameter_name))
	{}
}
