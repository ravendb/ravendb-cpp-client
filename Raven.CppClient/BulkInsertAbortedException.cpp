#include "stdafx.h"
#include "BulkInsertAbortedException.h"

namespace ravendb::client::exceptions::documents::bulkinsert
{
	BulkInsertAbortedException::BulkInsertAbortedException() = default;

	BulkInsertAbortedException::BulkInsertAbortedException(const std::string& message)
		: RavenException(message)
	{}
}
