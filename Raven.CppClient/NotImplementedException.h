#pragma once
#include <exception>

namespace ravendb::client::exceptions
{
	class NotImplementedException : public std::runtime_error
	{
	public:
		~NotImplementedException() = default;

		NotImplementedException();

		explicit NotImplementedException(const std::string& message);
	};
}
