#include "stdafx.h"
#include "IntersectMarkerToken.h"

namespace ravendb::client::documents::session::tokens
{
	const std::shared_ptr<IntersectMarkerToken> IntersectMarkerToken::INSTANCE = std::shared_ptr<IntersectMarkerToken>(new IntersectMarkerToken());

	void IntersectMarkerToken::write_to(std::ostringstream& writer) const
	{
		writer << ",";
	}
}