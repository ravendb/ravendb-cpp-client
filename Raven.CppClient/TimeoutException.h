#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions
{
	class TimeoutException : public RavenException
	{
	public:
		~TimeoutException() override = default;

		TimeoutException();

		explicit TimeoutException(const std::string& message);
	};
}
