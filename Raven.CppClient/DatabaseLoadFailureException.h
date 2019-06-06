#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::database
{
	class DatabaseLoadFailureException : public RavenException
	{
	public:
		~DatabaseLoadFailureException() override = default;

		DatabaseLoadFailureException();

		explicit DatabaseLoadFailureException(const std::string& message);
	};
}
