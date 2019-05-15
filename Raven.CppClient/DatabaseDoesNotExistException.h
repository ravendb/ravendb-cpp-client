#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::database
{
	class DatabaseDoesNotExistException : public RavenException
	{
	public:
		~DatabaseDoesNotExistException() override = default;

		DatabaseDoesNotExistException();

		explicit DatabaseDoesNotExistException(const std::string& message);
	};
}
