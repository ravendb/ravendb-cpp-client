#include "stdafx.h"
#include "BadResponseException.h"

namespace ravendb::client::exceptions
{
	BadResponseException::BadResponseException() = default;

	BadResponseException::BadResponseException(const std::string& message)
		: RavenException(message)
	{}
}
