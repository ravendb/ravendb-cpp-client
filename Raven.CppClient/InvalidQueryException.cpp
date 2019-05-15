#include "stdafx.h"
#include "InvalidQueryException.h"

namespace ravendb::client::exceptions
{
	InvalidQueryException::InvalidQueryException() = default;

	InvalidQueryException::InvalidQueryException(const std::string& message)
		: RavenException(message)
	{}
}
