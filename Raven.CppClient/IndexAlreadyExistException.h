#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	class IndexAlreadyExistException : public RavenException
	{
	public:
		~IndexAlreadyExistException() override = default;

		IndexAlreadyExistException();

		explicit IndexAlreadyExistException(const std::string& message);
	};
}
