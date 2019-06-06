#include "stdafx.h"
#include "ConflictException.h"

namespace ravendb::client::exceptions
{
	ConflictException::~ConflictException() = default;

	ConflictException::ConflictException() = default;

	ConflictException::ConflictException(const std::string& message)
		: RavenException(message)
	{}
}
