#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::server
{
	class ServerLoadFailureException : public RavenException
	{
	public:
		~ServerLoadFailureException() override = default;

		ServerLoadFailureException();

		explicit ServerLoadFailureException(const std::string& message);
	};
}
