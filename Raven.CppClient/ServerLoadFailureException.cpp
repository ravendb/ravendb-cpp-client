#include "stdafx.h"
#include "ServerLoadFailureException.h"

namespace ravendb::client::exceptions::server
{
	ServerLoadFailureException::ServerLoadFailureException() = default;

	ServerLoadFailureException::ServerLoadFailureException(const std::string& message)
		: RavenException(message)
	{}
}
