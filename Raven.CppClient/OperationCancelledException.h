#pragma once

namespace ravendb::client::primitives
{
	class OperationCancelledException : public std::runtime_error
	{
	public:
		OperationCancelledException()
			: OperationCancelledException("")
		{}

		OperationCancelledException(const char* msg)
			: std::runtime_error(msg)
		{}

		OperationCancelledException(const std::string& msg)
			: std::runtime_error(msg)
		{}
	};
}
