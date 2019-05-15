#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::database
{
	class DatabaseConcurrentLoadTimeoutException : public RavenException
	{
	public:
		~DatabaseConcurrentLoadTimeoutException() override = default;

		DatabaseConcurrentLoadTimeoutException();

		explicit DatabaseConcurrentLoadTimeoutException(const std::string& message);
	};
}
