#include "stdafx.h"
#include "IndexDoesNotExistException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	IndexDoesNotExistException::IndexDoesNotExistException() = default;

	IndexDoesNotExistException::IndexDoesNotExistException(const std::string& message)
		: RavenException(message)
	{}
}
