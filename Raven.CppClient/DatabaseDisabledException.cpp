#include "stdafx.h"
#include "DatabaseDisabledException.h"

namespace ravendb::client::exceptions::database
{
	DatabaseDisabledException::DatabaseDisabledException() = default;

	DatabaseDisabledException::DatabaseDisabledException(const std::string& message)
		: RavenException(message)
	{}
}
