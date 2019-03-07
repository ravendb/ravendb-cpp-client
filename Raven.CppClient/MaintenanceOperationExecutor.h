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
		MaintenanceOperationExecutor(std::shared_ptr<DocumentStore> store)
		{}
		//TODO implement !
	};
}
