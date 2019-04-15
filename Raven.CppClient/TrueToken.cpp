#include "stdafx.h"
#include "TrueToken.h"

namespace ravendb::client::documents::session::tokens
{
	const std::shared_ptr<TrueToken> TrueToken::INSTANCE = std::shared_ptr<TrueToken>(new TrueToken());

	void TrueToken::write_to(std::ostringstream& writer) const
	{
		writer << "true";
	}
}
