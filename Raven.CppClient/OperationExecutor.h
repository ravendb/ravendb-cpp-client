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
		OperationExecutor(const IDocumentStore&)
		{}
		~OperationExecutor() = default;
		//TODO complete it!
	};
	
}
