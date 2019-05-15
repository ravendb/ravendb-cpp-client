#include "stdafx.h"
#include "AuthenticationException.h"

namespace ravendb::client::exceptions::security
{
	AuthenticationException::AuthenticationException() = default;

	AuthenticationException::AuthenticationException(const std::string& message)
		: SecurityException(message)
	{}
}
