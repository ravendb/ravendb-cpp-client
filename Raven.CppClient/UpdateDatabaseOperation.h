#pragma once
#include "IServerOperation.h"
#include "DatabasePutResult.h"
#include "DatabaseRecord.h"

namespace ravendb::client::serverwide::operations
{
	class UpdateDatabaseOperation : public IServerOperation<DatabasePutResult>
	{
	private:
		const DatabaseRecord _database_record;
		const int64_t _etag;

	public:
		~UpdateDatabaseOperation() override;

		UpdateDatabaseOperation(DatabaseRecord database_record, int64_t etag);

		std::unique_ptr<http::RavenCommand<DatabasePutResult>> get_command(std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;
	};
}
