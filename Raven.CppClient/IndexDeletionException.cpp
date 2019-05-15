#include "stdafx.h"
#include "IndexDeletionException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	IndexDeletionException::IndexDeletionException() = default;

	IndexDeletionException::IndexDeletionException(const std::string& message)
		: RavenException(message)
	{}
}
