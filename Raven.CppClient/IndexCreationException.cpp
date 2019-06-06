#include "stdafx.h"
#include "IndexCreationException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	IndexCreationException::IndexCreationException() = default;

	IndexCreationException::IndexCreationException(const std::string& message)
		: RavenException(message)
	{}
}
