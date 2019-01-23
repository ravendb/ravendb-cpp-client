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

		operations::LoadOperation load_impl(const std::vector<std::reference_wrapper<const std::string>>& ids);

		void load_internal(const std::vector<std::reference_wrapper<const std::string>>& ids,
			operations::LoadOperation& operation);

	public:
		~DocumentSessionImpl() override = default;

		DocumentSessionImpl(DocumentStoreBase& document_store,/* UUID id,*/ SessionOptions options);

		template<typename T>
		std::shared_ptr<T> load(const std::string& id);

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		DocumentsByIdsMap<T> load(InputIt first, InputIt last);

		bool exists(const std::string& id);

		void save_changes();
	};

	template <typename T>
	std::shared_ptr<T> DocumentSessionImpl::load(const std::string& id)
	{
		return id.empty() ? nullptr : load_impl(id).get_document<T>();
	}

	template <typename T, typename InputIt>
	DocumentsByIdsMap<T> DocumentSessionImpl::load(InputIt first, InputIt last)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIt>::value_type, std::string>, "invalid iterator type");

		std::vector<std::reference_wrapper<const std::string>> ids{};
		std::transform(first, last, std::back_inserter(ids), [](const std::string& id) {return std::cref(id); });

		return load_impl(ids).get_documents<T>();
	}

}
