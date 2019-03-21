#pragma once

namespace ravendb::client::documents
{
	struct IDocumentStore;
}

namespace ravendb::client::documents::operations
{
	class OperationExecutor
	{
	public:
		OperationExecutor(std::shared_ptr<IDocumentStore> store)
		{}
		~OperationExecutor() = default;
		//TODO complete it!
	};
	
}
