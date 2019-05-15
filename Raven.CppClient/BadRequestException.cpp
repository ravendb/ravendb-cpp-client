#include "stdafx.h"
#include "BadRequestException.h"

namespace ravendb::client::exceptions
{
	BadRequestException::BadRequestException() = default;

	BadRequestException::BadRequestException(const std::string& message)
		: RavenException(message)
	{}
}
