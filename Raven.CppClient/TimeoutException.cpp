#include "stdafx.h"
#include "TimeoutException.h"

namespace ravendb::client::exceptions
{
	TimeoutException::TimeoutException() = default;

	TimeoutException::TimeoutException(const std::string& message)
		: RavenException(message)
	{}
}
