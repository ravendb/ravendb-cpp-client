#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::bulkinsert
{
	class BulkInsertAbortedException : public RavenException
	{
	public:
		~BulkInsertAbortedException() override = default;

		BulkInsertAbortedException();

		explicit BulkInsertAbortedException(const std::string& message);
	};
}
