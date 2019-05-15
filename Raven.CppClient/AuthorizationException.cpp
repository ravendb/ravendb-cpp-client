#include "stdafx.h"
#include "AuthorizationException.h"

namespace ravendb::client::exceptions::security
{
	AuthorizationException::AuthorizationException() = default;

	AuthorizationException::AuthorizationException(const std::string& message)
		: SecurityException(message)
	{}
}
