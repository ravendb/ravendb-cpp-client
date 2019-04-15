#pragma once
#include "DocumentSessionImpl.h"
#include "LoaderWithInclude.h"

namespace ravendb::client::documents
{
	namespace session
	{
		class AdvancedSessionOperations;
	}
	namespace queries
	{
		class Query;
	}
}
namespace ravendb::client::documents::session
{
	class DocumentSession
	{
	private:
		std::shared_ptr<DocumentSessionImpl> _session_impl;

	public:
		//WARNING for unit test ONLY
		std::shared_ptr<DocumentSessionImpl> get_session_implementation() const;

		~DocumentSession();

		explicit DocumentSession(std::shared_ptr<DocumentSessionImpl> session_impl);

		AdvancedSessionOperations advanced();

		const AdvancedSessionOperations advanced() const;

		template<typename T>
		void delete_document(std::shared_ptr<T> entity);

		template<typename T>
		void delete_document(const std::string& id, const std::string& expected_change_vector = {});

		void delete_document(const std::string& id, const std::string& expected_change_vector = {},
		                     const DocumentInfo::ToJsonConverter& to_json = {});

		void save_changes();

		template<typename T>
		void store(std::shared_ptr<T> entity,
		           const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		void store(std::shared_ptr<T> entity, std::string id,
		           const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		void store(std::shared_ptr<T> entity, const char* id,
		           const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		void store(std::shared_ptr<T> entity,
		           const std::optional<std::string>& change_vector,
		           const std::optional<std::string>& id = {},
		           const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		loaders::LoaderWithInclude include(const std::string& path);

		template<typename T>
		std::shared_ptr<T> load(const std::string& id,
		                        const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
		                        const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		DocumentsByIdsMap<T> load(InputIt first, InputIt last,
		                          const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
		                          const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		//load by vector of ids (std::string)
		template<typename T>
		DocumentsByIdsMap<T> load(const std::vector<std::string>& ids,
		                          const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
		                          const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> query();

		template<typename T>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> query(const queries::Query& collection_or_index_name);
	};

	template <typename T>
	void DocumentSession::delete_document(std::shared_ptr<T> entity)
	{
		_session_impl->delete_document(entity);
	}

	template <typename T>
	void DocumentSession::delete_document(const std::string& id, const std::string& expected_change_vector)
	{
		_session_impl->delete_document<T>(id, expected_change_vector);
	}

	template <typename T>
	void DocumentSession::store(std::shared_ptr<T> entity, const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		_session_impl->store(entity, to_json);
	}

	template <typename T>
	void DocumentSession::store(std::shared_ptr<T> entity, std::string id,
	                            const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		_session_impl->store(entity, id, to_json);
	}

	template <typename T>
	void DocumentSession::store(std::shared_ptr<T> entity, const char* id,
	                            const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		this->store(entity, std::string(id), to_json);
	}

	template <typename T>
	void DocumentSession::store(std::shared_ptr<T> entity, const std::optional<std::string>& change_vector,
	                            const std::optional<std::string>& id,
	                            const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		_session_impl->store(entity, change_vector, id, to_json);
	}

	template <typename T>
	std::shared_ptr<T> DocumentSession::load(const std::string& id,
	                                         const std::optional<DocumentInfo::FromJsonConverter>& from_json,
	                                         const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		return _session_impl->load<T>(id, from_json, to_json);
	}

	template <typename T, typename InputIt>
	DocumentsByIdsMap<T> DocumentSession::load(InputIt first, InputIt last,
	                                           const std::optional<DocumentInfo::FromJsonConverter>& from_json,
	                                           const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		return _session_impl->load<T, InputIt>(first, last, from_json, to_json);
	}

	template <typename T>
	DocumentsByIdsMap<T> DocumentSession::load(const std::vector<std::string>& ids,
	                                           const std::optional<DocumentInfo::FromJsonConverter>& from_json,
	                                           const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		return _session_impl->load<T>(ids.cbegin(), ids.cend(), from_json, to_json);
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentSession::query()
	{
		return _session_impl->query<T>();
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentSession::query(const queries::Query& collection_or_index_name)
	{
		return _session_impl->query<T>(std::move(collection_or_index_name));
	}
}
