#include "stdafx.h"
#include "UpdateDatabaseOperation.h"
#include "CreateDatabaseOperation.h"

namespace ravendb::client::serverwide::operations
{
	UpdateDatabaseOperation::~UpdateDatabaseOperation() = default;

	UpdateDatabaseOperation::UpdateDatabaseOperation(DatabaseRecord database_record, int64_t etag)
		: _database_record(std::move(database_record))
		, _etag(etag)
	{}

	std::unique_ptr<http::RavenCommand<DatabasePutResult>> UpdateDatabaseOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<CreateDatabaseOperation::CreateDatabaseCommand>(conventions, _database_record, 1, _etag);
	}
}
