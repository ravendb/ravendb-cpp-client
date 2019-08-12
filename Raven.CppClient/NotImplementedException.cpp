#include "stdafx.h"
#include "NotImplementedException.h"

namespace ravendb::client::exceptions
{
	NotImplementedException::NotImplementedException()
		: std::runtime_error("")
	{}

	NotImplementedException::NotImplementedException(const std::string& message)
		: std::runtime_error(message)
	{}
}
