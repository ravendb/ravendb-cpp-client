#include "stdafx.h"
#include "CommandExecutionException.h"

namespace ravendb::client::exceptions::cluster
{
	CommandExecutionException::CommandExecutionException() = default;

	CommandExecutionException::CommandExecutionException(const std::string& message)
		: RavenException(message)
	{}
}
