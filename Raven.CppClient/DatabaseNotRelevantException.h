#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::database
{
	class DatabaseNotRelevantException : public RavenException
	{
	public:
		~DatabaseNotRelevantException() override = default;

		DatabaseNotRelevantException();

		explicit DatabaseNotRelevantException(const std::string& message);
	};
}
