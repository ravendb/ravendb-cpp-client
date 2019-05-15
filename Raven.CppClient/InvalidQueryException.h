#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions
{
	class InvalidQueryException : public RavenException
	{
	public:
		~InvalidQueryException() override = default;

		InvalidQueryException();

		explicit InvalidQueryException(const std::string& message);
	};
}
