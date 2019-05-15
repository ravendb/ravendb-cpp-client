#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions
{
	class ClientVersionMismatchException : public RavenException
	{
	public:
		~ClientVersionMismatchException() override = default;

		ClientVersionMismatchException();

		explicit ClientVersionMismatchException(const std::string& message);
	};
}
