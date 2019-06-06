#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions
{
	class BadRequestException : public RavenException
	{
	public:
		~BadRequestException() override = default;

		BadRequestException();

		explicit BadRequestException(const std::string& message);
	};
}
