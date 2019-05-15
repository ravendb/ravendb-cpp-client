#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions
{
	class BadResponseException : public RavenException
	{
	public:
		~BadResponseException() override = default;

		BadResponseException();

		explicit BadResponseException(const std::string& message);
	};
}
