#include "stdafx.h"
#include "NoLeaderException.h"

namespace ravendb::client::exceptions::cluster
{
	NoLeaderException::NoLeaderException() = default;

	NoLeaderException::NoLeaderException(const std::string& message)
		: RavenException(message)
	{}
}
