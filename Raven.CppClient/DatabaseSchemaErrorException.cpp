#include "stdafx.h"
#include "DatabaseSchemaErrorException.h"

namespace ravendb::client::exceptions::database
{
	DatabaseSchemaErrorException::DatabaseSchemaErrorException() = default;

	DatabaseSchemaErrorException::DatabaseSchemaErrorException(const std::string& message)
		: RavenException(message)
	{}
}
