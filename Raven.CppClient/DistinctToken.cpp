#include "stdafx.h"
#include "DistinctToken.h"

namespace ravendb::client::documents::session::tokens
{
	const std::shared_ptr<DistinctToken> DistinctToken::INSTANCE = std::shared_ptr<DistinctToken>(new DistinctToken());

	void DistinctToken::write_to(std::ostringstream& writer) const
	{
		writer << "distinct";
	}
}
