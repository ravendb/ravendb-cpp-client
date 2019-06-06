#include "stdafx.h"
#include "IndexInvalidException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	IndexInvalidException::IndexInvalidException() = default;

	IndexInvalidException::IndexInvalidException(const std::string& message)
		: RavenException(message)
	{}
}
