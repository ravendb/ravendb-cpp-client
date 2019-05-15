#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions
{
	class ConflictException : public RavenException
	{
	public:
		~ConflictException() override = 0;

		ConflictException();

		explicit ConflictException(const std::string& message);
	};
}
