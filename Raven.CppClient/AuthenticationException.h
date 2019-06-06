#pragma once
#include "SecurityException.h"

namespace ravendb::client::exceptions::security
{
	class AuthenticationException : public SecurityException
	{
	public:
		~AuthenticationException() override = default;

		AuthenticationException();

		explicit AuthenticationException(const std::string& message);
	};
}
