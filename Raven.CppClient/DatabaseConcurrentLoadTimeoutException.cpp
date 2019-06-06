#include "stdafx.h"
#include "DatabaseConcurrentLoadTimeoutException.h"

namespace ravendb::client::exceptions::database
{
	DatabaseConcurrentLoadTimeoutException::DatabaseConcurrentLoadTimeoutException() = default;

	DatabaseConcurrentLoadTimeoutException::DatabaseConcurrentLoadTimeoutException(const std::string& message)
		: RavenException(message)
	{}
}
