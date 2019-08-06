#include "stdafx.h"
#include "UnsupportedOperationException.h"

namespace ravendb::client::exceptions
{
	UnsupportedOperationException::UnsupportedOperationException()
		: std::runtime_error("")
	{}

	UnsupportedOperationException::UnsupportedOperationException(const std::string& message)
		: std::runtime_error(message)
	{}
}
