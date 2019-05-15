#include "stdafx.h"
#include "LicenseActivationException.h"

namespace ravendb::client::exceptions::commercial
{
	LicenseActivationException::LicenseActivationException() = default;

	LicenseActivationException::LicenseActivationException(const std::string& message)
		: RavenException(message)
	{}
}
