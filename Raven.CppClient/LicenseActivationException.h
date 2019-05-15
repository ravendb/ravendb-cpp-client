#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::commercial
{
	class LicenseActivationException : public RavenException
	{
	public:
		~LicenseActivationException() override = default;

		LicenseActivationException();

		explicit LicenseActivationException(const std::string& message);
	};
}
