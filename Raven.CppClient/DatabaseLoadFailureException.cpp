#include "stdafx.h"
#include "DatabaseLoadFailureException.h"

namespace ravendb::client::exceptions::database
{
	DatabaseLoadFailureException::DatabaseLoadFailureException() = default;

	DatabaseLoadFailureException::DatabaseLoadFailureException(const std::string& message)
		: RavenException(message)
	{}
}
