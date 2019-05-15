#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::database
{
	class DatabaseSchemaErrorException : public RavenException
	{
	public:
		~DatabaseSchemaErrorException() override = default;

		DatabaseSchemaErrorException();

		explicit DatabaseSchemaErrorException(const std::string& message);
	};
}
