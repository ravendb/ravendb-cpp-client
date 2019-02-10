#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "DocumentsByIdsMap.h"
#include "LoadOperation.h"
#include "RawDocumentQuery.h"

namespace ravendb::client::documents::session
{
	namespace loaders
	{
		class LoaderWithInclude;
	}

	class DocumentSessionImpl : public InMemoryDocumentSessionOperations
	{
	private:
		operations::LoadOperation load_impl(const std::string& id);

		operations::LoadOperation load_impl(const std::vector<std::reference_wrapper<const std::string>>& ids);

		operations::LoadOperation load_impl(const std::vector<std::reference_wrapper<const std::string>>& ids,
		const std::vector<std::string>& includes);

		void load_internal(const std::vector<std::reference_wrapper<const std::string>>& ids,
			operations::LoadOperation& operation);

	public:
		~DocumentSessionImpl() override = default;

		DocumentSessionImpl(DocumentStoreBase& document_store,/* UUID id,*/ SessionOptions options);

		loaders::LoaderWithInclude include(const std::string& path);

		template <typename T>
		DocumentsByIdsMap<T> load_internal(const std::vector<std::reference_wrapper<const std::string>>& ids,
			const std::vector<std::string>& includes,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json,
			const std::optional<DocumentInfo::ToJsonConverter>& to_json);

		template<typename T>
		std::shared_ptr<T> load(const std::string& id, 
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		DocumentsByIdsMap<T> load(InputIt first, InputIt last,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		bool exists(const std::string& id);

		void save_changes();

		std::shared_ptr<RawDocumentQuery> raw_query(const std::string& query)
		{
			return RawDocumentQuery::create(*this, query);
		}
	};

	template <typename T>
	DocumentsByIdsMap<T> DocumentSessionImpl::load_internal(
		const std::vector<std::reference_wrapper<const std::string>>& ids,
		const std::vector<std::string>& includes,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		return load_impl(ids, includes).get_documents<T>(from_json, to_json);
	}

	template <typename T>
	std::shared_ptr<T> DocumentSessionImpl::load(const std::string& id,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		return id.empty() ? nullptr : load_impl(id).get_document<T>(from_json, to_json);
	}

	template <typename T, typename InputIt>
	DocumentsByIdsMap<T> DocumentSessionImpl::load(InputIt first, InputIt last,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIt>::value_type, std::string>, "invalid iterator type");

		std::vector<std::reference_wrapper<const std::string>> ids{};
		std::transform(first, last, std::back_inserter(ids), [](const std::string& id) {return std::cref(id); });

		return load_impl(ids).get_documents<T>(from_json, to_json);
	}

}
