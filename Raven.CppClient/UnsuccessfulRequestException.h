#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions
{
	class UnsuccessfulRequestException : public RavenException
	{
	public:
		~UnsuccessfulRequestException() override = default;

		explicit UnsuccessfulRequestException(const std::string& message);
	};
}
