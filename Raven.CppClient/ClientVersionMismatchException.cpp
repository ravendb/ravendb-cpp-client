#include "stdafx.h"
#include "ClientVersionMismatchException.h"

namespace ravendb::client::exceptions
{
	ClientVersionMismatchException::ClientVersionMismatchException() = default;

	ClientVersionMismatchException::ClientVersionMismatchException(const std::string& message)
		: RavenException(message)
	{}
}
