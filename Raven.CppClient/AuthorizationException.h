#pragma once
#include "SecurityException.h"

namespace ravendb::client::exceptions::security
{
	class AuthorizationException : public SecurityException
	{
	public:
		~AuthorizationException() override = default;

		AuthorizationException();

		explicit AuthorizationException(const std::string& message);
	};
}
