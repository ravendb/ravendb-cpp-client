#include "stdafx.h"
#include "OpenSubclauseToken.h"

namespace ravendb::client::documents::session::tokens
{
	const std::shared_ptr<OpenSubclauseToken> OpenSubclauseToken::INSTANCE = 
		std::shared_ptr<OpenSubclauseToken>(new OpenSubclauseToken());

	void OpenSubclauseToken::write_to(std::ostringstream& writer) const
	{
		writer << "(";
	}
}
