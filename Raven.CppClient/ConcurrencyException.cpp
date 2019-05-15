#include "stdafx.h"
#include "ConcurrencyException.h"

namespace ravendb::client::exceptions
{
	ConcurrencyException::ConcurrencyException() = default;

	ConcurrencyException::ConcurrencyException(const std::string& message)
		:	ConcurrencyException(message)
	{}
}
