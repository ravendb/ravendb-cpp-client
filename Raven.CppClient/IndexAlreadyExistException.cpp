#include "stdafx.h"
#include "IndexAlreadyExistException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	IndexAlreadyExistException::IndexAlreadyExistException() = default;

	IndexAlreadyExistException::IndexAlreadyExistException(const std::string& message)
		: RavenException(message)
	{}
}
