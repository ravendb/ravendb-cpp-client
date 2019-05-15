#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents
{
	class DocumentDoesNotExistException : public RavenException
	{
	public:
		~DocumentDoesNotExistException() override = default;

		DocumentDoesNotExistException();

		explicit DocumentDoesNotExistException(const std::string& id);
	};
}
