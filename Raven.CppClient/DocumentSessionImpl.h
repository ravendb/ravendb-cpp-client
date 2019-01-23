#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "BatchCommand.h"
#include "LoadOperation.h"

namespace ravendb::client::documents::session
{
	class DocumentSessionImpl : public InMemoryDocumentSessionOperations
	{
	private:
		operations::LoadOperation load_impl(const std::string& id);
	public:
		~DocumentSessionImpl() override = default;

		DocumentSessionImpl(DocumentStoreBase& document_store,/* UUID id,*/ SessionOptions options);

		template<typename T>
		std::shared_ptr<T> load(const std::string& id);

		bool exists(const std::string& id);

		void save_changes();
	};

	template <typename T>
	std::shared_ptr<T> DocumentSessionImpl::load(const std::string& id)
	{
		return load_impl(id).get_document<T>();
	}
}
