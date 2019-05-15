#include "stdafx.h"
#include "ChangeProcessingException.h"

namespace ravendb::client::exceptions::changes
{
	ChangeProcessingException::ChangeProcessingException() = default;

	ChangeProcessingException::ChangeProcessingException(const std::string& message)
		: RavenException(message)
	{}
}
