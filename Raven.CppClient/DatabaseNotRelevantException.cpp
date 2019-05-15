#include "stdafx.h"
#include "DatabaseNotRelevantException.h"

namespace ravendb::client::exceptions::database
{
	DatabaseNotRelevantException::DatabaseNotRelevantException() = default;

	DatabaseNotRelevantException::DatabaseNotRelevantException(const std::string& message)
		: RavenException(message)
	{}
}
