#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::cluster
{
	class CommandExecutionException : public RavenException
	{
	public:
		~CommandExecutionException() override = default;

		CommandExecutionException();

		explicit CommandExecutionException(const std::string& message);
	};
}
