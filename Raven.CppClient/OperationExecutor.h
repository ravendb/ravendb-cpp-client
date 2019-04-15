#pragma once

namespace ravendb::client::documents
{
	struct IDocumentStore;
}

namespace ravendb::client::documents::operations
{
	class OperationExecutor
	{
	private:
		std::weak_ptr<OperationExecutor> _weak_this;
		const std::weak_ptr<IDocumentStore> _store;

	protected:
		explicit OperationExecutor(std::shared_ptr<IDocumentStore> store)
			: _store(store)
		{}

	public:
		~OperationExecutor() = default;

		static std::shared_ptr<OperationExecutor> create(std::shared_ptr<IDocumentStore> store)
		{
			auto object = std::shared_ptr<OperationExecutor>(new OperationExecutor(store));
			object->_weak_this = object;
			return object;
		}

		//TODO complete it!
	};
	
}
