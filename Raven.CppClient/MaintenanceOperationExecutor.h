#pragma once

namespace ravendb::client::documents
{
	class DocumentStore;
}

namespace ravendb::client::documents::operations
{
	class MaintenanceOperationExecutor
	{
	public:
		MaintenanceOperationExecutor(const DocumentStore&)
		{}
		//TODO implement !
	};
}
