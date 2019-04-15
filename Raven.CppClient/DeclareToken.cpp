#include "stdafx.h"
#include "DeclareToken.h"

namespace ravendb::client::documents::session::tokens
{
	DeclareToken::DeclareToken(std::string name, std::string body, std::optional<std::string> parameters)
		: _name(std::move(name))
		, _body(std::move(body))
		, _parameters(std::move(parameters))
	{}

	std::shared_ptr<DeclareToken> DeclareToken::create(std::string name, std::string body, std::optional<std::string> parameters)
	{
		return std::shared_ptr<DeclareToken>(new DeclareToken(std::move(name), std::move(body), std::move(parameters)));
	}

	void DeclareToken::write_to(std::ostringstream& writer) const
	{
		writer
			<< "declare "
			<< "function "
			<< _name
			<< "(" << (_parameters ? *_parameters : " ") << ")"
			<< "{" << "\r\n"
			<< _body << "\r\n"
			<< "}" << "\r\n";
	}
}
