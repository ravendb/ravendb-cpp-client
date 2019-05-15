#include "stdafx.h"
#include "CertificateNameMismatchException.h"

namespace ravendb::client::exceptions::security
{
	CertificateNameMismatchException::CertificateNameMismatchException() = default;

	CertificateNameMismatchException::CertificateNameMismatchException(const std::string& message)
		: AuthenticationException(message)
	{}
}
