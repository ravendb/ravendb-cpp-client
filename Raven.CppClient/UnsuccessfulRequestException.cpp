#include "stdafx.h"
#include "UnsuccessfulRequestException.h"

namespace ravendb::client::exceptions
{
	UnsuccessfulRequestException::UnsuccessfulRequestException(const std::string& message)
		: RavenException(message + " Request to a server has failed.")
	{}
}
