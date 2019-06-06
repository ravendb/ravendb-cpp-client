#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::database
{
	class DatabaseLoadTimeoutException : public RavenException
	{
	public:
		~DatabaseLoadTimeoutException() override = default;

		DatabaseLoadTimeoutException();

		explicit DatabaseLoadTimeoutException(const std::string& message);
	};
}
