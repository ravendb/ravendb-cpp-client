#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::bulkinsert
{
	class BulkInsertProtocolViolationException : public RavenException
	{
	public:
		~BulkInsertProtocolViolationException() override = default;

		BulkInsertProtocolViolationException();

		explicit BulkInsertProtocolViolationException(const std::string& message);
	};
}
