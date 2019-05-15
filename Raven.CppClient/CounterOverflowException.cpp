#include "stdafx.h"
#include "CounterOverflowException.h"

namespace ravendb::client::exceptions::documents::counters
{
	CounterOverflowException::CounterOverflowException() = default;

	CounterOverflowException::CounterOverflowException(const std::string& message)
		: RavenException(message)
	{}
}
