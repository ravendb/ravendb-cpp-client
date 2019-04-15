#include "stdafx.h"
#include "NegateToken.h"

namespace ravendb::client::documents::session::tokens
{
	const std::shared_ptr<NegateToken> NegateToken::INSTANCE = std::shared_ptr<NegateToken>(new NegateToken());;

	void NegateToken::write_to(std::ostringstream& writer) const
	{
		writer << "not";
	}
}
