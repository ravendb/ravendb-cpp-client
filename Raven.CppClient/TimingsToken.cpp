#include "stdafx.h"
#include "TimingsToken.h"

namespace ravendb::client::documents::session::tokens
{
	const std::shared_ptr<TimingsToken> TimingsToken::INSTANCE = std::shared_ptr<TimingsToken>(new TimingsToken);

	void TimingsToken::write_to(std::ostringstream& writer) const
	{
		writer << "timings()";
	}
}
