#include "stdafx.h"
#include "NonUniqueObjectException.h"

namespace ravendb::client::exceptions::documents::session
{
	NonUniqueObjectException::NonUniqueObjectException() = default;

	NonUniqueObjectException::NonUniqueObjectException(const std::string& message)
		: RavenException(message)
	{}
}
