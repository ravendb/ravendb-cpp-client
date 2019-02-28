#include "stdafx.h"
#include "DeclareToken.h"

namespace ravendb::client::documents::session::tokens
{
	DeclareToken::DeclareToken(std::string name, std::string body, std::string parameters)
		: _name(std::move(name))
		, _body(std::move(body))
		, _parameters(std::move(parameters))
	{}

	void DeclareToken::write_to(std::ostringstream& writer) const
	{
		writer
			<< "declare "
			<< "function "
			<< _name
			<< "(" << _parameters << ")"
			<< "{" << "\r\n"
			<< _body << "\r\n"
			<< "}" << "\r\n";
	}
}
