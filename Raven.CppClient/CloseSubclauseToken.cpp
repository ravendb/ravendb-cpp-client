#include "stdafx.h"
#include "CloseSubclauseToken.h"

namespace ravendb::client::documents::session::tokens
{
	const std::shared_ptr<CloseSubclauseToken> CloseSubclauseToken::INSTANCE = std::shared_ptr<CloseSubclauseToken>(new CloseSubclauseToken());

	void CloseSubclauseToken::write_to(std::ostringstream& writer) const
	{
		writer << ")";
	}
}
