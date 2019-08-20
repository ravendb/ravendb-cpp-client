#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::security
{
	class SecurityException : public RavenException
	{
	public:
		~SecurityException() override = default;

		SecurityException();

		explicit SecurityException(const std::string& message);
	};
}
