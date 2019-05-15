#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::counters
{
	class CounterOverflowException : RavenException
	{
	public:
		~CounterOverflowException() override = default;

		CounterOverflowException();

		explicit CounterOverflowException(const std::string& message);
	};
}
