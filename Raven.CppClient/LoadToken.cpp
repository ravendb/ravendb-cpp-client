#include "stdafx.h"
#include "LoadToken.h"

namespace ravendb::client::documents::session::tokens
{
	LoadToken::LoadToken(std::optional<std::string> argument_param, std::optional<std::string> alias_param)
		: argument(std::move(argument_param))
		, alias(std::move(alias_param))
	{}

	std::shared_ptr<LoadToken> LoadToken::create(std::optional<std::string> argument_param,
		std::optional<std::string> alias_param)
	{
		return std::shared_ptr<LoadToken>(new LoadToken(std::move(argument_param), std::move(alias_param)));
	}

	void LoadToken::write_to(std::ostringstream& writer) const
	{
		writer << (argument ? *argument : "");
		writer << " as ";
		writer << (alias ? *alias : "");
	}
}
