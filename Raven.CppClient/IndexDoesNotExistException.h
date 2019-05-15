#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	class IndexDoesNotExistException : public RavenException
	{
	public:
		~IndexDoesNotExistException() override = default;

		IndexDoesNotExistException();

		explicit IndexDoesNotExistException(const std::string& message);
	};
}
