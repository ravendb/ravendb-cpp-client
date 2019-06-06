#include "stdafx.h"
#include "SecurityException.h"

namespace ravendb::client::exceptions::security
{
	SecurityException::SecurityException() = default;

	SecurityException::SecurityException(const std::string& message)
		: RavenException(message)
	{}
}
