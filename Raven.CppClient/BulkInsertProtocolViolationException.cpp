#include "stdafx.h"
#include "BulkInsertProtocolViolationException.h"

namespace ravendb::client::exceptions::documents::bulkinsert
{
	BulkInsertProtocolViolationException::BulkInsertProtocolViolationException() = default;

	BulkInsertProtocolViolationException::BulkInsertProtocolViolationException(const std::string& message)
		: RavenException(message)
	{}
}
