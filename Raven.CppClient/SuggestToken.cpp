#include "stdafx.h"
#include "SuggestToken.h"

namespace ravendb::client::documents::session::tokens
{
	SuggestToken::SuggestToken(std::string field_name, std::string term_parameter_name,
		std::optional<std::string> options_parameter_name)
		: _field_name(std::move(field_name))
		, _term_parameter_name(std::move(term_parameter_name))
		, _options_parameter_name(std::move(options_parameter_name))
	{}

	std::shared_ptr<SuggestToken> SuggestToken::create(std::string field_name, std::string term_parameter_name,
		std::optional<std::string> options_parameter_name)
	{
		return std::shared_ptr<SuggestToken>(new SuggestToken(std::move(field_name),
			std::move(term_parameter_name), std::move(options_parameter_name)));
	}

	void SuggestToken::write_to(std::ostringstream& writer) const
	{
		writer << "suggest(" << _field_name << ", $" << _term_parameter_name;

		if(_options_parameter_name)
		{
			writer << ", $" << *_options_parameter_name;
		}

		writer << ")";
	}
}
