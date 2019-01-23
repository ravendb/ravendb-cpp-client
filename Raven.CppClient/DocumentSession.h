#pragma once
#include "DocumentSessionImpl.h"
#include "AdvancedDocumentSessionOperations.h"

namespace ravendb::client::documents::session
{
	class DocumentSession
	{
	private:
		std::shared_ptr<DocumentSessionImpl> _session_impl;

	public:
		~DocumentSession() = default;

		explicit DocumentSession(std::shared_ptr<DocumentSessionImpl> session_impl)
			: _session_impl(session_impl)
		{}

		AdvancedDocumentSessionOperations advanced()
		{
			return AdvancedDocumentSessionOperations(_session_impl);
		}

		const AdvancedDocumentSessionOperations advanced() const
		{
			return AdvancedDocumentSessionOperations(_session_impl);
		}

		template<typename T>
		void delete_document(std::shared_ptr<T> entity)
		{
			_session_impl->delete_document(entity);
		}

		void delete_document(const std::string& id, const std::string& expected_change_vector = {})
		{
			_session_impl->delete_document(id, expected_change_vector);
		}

		void save_changes()
		{
			_session_impl->save_changes();
		}

		template<typename T>
		void store(std::shared_ptr<T> entity, std::optional<DocumentInfo::ToJsonConverter> to_json = {})
		{
			_session_impl->store(entity, to_json);
		}

		template<typename T>
		void store(std::shared_ptr<T> entity, std::string id, std::optional<DocumentInfo::ToJsonConverter> to_json = {})
		{
			_session_impl->store(entity, id, to_json);
		}

		template<typename T>
		void store(std::shared_ptr<T> entity, std::optional<std::string> id = {},
			std::optional<std::string> change_vector = {}, std::optional<DocumentInfo::ToJsonConverter> to_json = {})
		{
			_session_impl->store(entity, id, change_vector, to_json);
		}

		template<typename T>
		std::shared_ptr<T> load(const std::string& id)
		{
			return _session_impl->load<T>(id);
		}
	};
}
