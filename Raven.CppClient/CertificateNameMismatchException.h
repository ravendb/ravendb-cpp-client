#pragma once
#include "AuthenticationException.h"

namespace ravendb::client::exceptions::security
{
	class CertificateNameMismatchException : public AuthenticationException
	{
	public:
		~CertificateNameMismatchException() override = default;

		CertificateNameMismatchException();

		explicit CertificateNameMismatchException(const std::string& message);
	};
}
