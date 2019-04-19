#include "stdafx.h"
#include "ExplanationToken.h"

namespace ravendb::client::documents::session::tokens
{
	ExplanationToken::ExplanationToken(std::optional<std::string> options_parameter_name)
		: _options_parameter_name(std::move(options_parameter_name))
	{}

	std::shared_ptr<ExplanationToken> ExplanationToken::create(std::optional<std::string> options_parameter_name)
	{
		return std::shared_ptr<ExplanationToken>(new ExplanationToken(std::move(options_parameter_name)));
	}

	void ExplanationToken::write_to(std::ostringstream& writer) const
	{
		writer << "explanations(";
		if(_options_parameter_name)
		{
			writer << "$" << *_options_parameter_name;
		}
		writer << ")";
	}
}
