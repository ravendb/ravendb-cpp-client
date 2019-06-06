#include "stdafx.h"
#include "DatabaseLoadTimeoutException.h"

namespace ravendb::client::exceptions::database
{
	DatabaseLoadTimeoutException::DatabaseLoadTimeoutException() = default;

	DatabaseLoadTimeoutException::DatabaseLoadTimeoutException(const std::string& message)
		: RavenException(message)
	{}
}
