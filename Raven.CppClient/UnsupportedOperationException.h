#pragma once
#include <exception>

namespace ravendb::client::exceptions
{
	class UnsupportedOperationException : public std::runtime_error
	{
	public:
		~UnsupportedOperationException() = default;

		UnsupportedOperationException();

		explicit UnsupportedOperationException(const std::string& message);
	};
}
