#include "stdafx.h"
#include "DatabaseDoesNotExistException.h"

namespace ravendb::client::exceptions::database
{
	DatabaseDoesNotExistException::DatabaseDoesNotExistException() = default;

	DatabaseDoesNotExistException::DatabaseDoesNotExistException(const std::string& message)
		: RavenException(message)
	{}
}
