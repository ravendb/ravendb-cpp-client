#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "GetDocumentsResult.h"
#include "GetDocumentsCommand.h"
#include "DocumentsByIdsMap.h"

namespace ravendb::client::documents::session::operations
{
	class LoadOperation
	{
	private:
		const std::weak_ptr<InMemoryDocumentSessionOperations> _session;

		std::vector<std::string> _ids{};

		std::vector<std::string> _includes{};

		std::vector<std::string> _ids_to_check_on_server{};

		std::shared_ptr<commands::GetDocumentsResult> _current_load_results{};

	public:
		LoadOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session);

		std::unique_ptr<http::RavenCommand<commands::GetDocumentsResult>> create_request() const;

		LoadOperation& with_includes(std::vector<std::string> includes);

		LoadOperation& by_id(const std::string& id);

		LoadOperation& by_ids(const std::vector<std::reference_wrapper<const std::string>>& ids);

		template<typename T>
		std::shared_ptr<T> get_document(
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		std::shared_ptr<T> get_document(
			const std::string& id,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		DocumentsByIdsMap<T> get_documents(
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			DocumentsByIdsMap<T> results{};

			if(_session.lock()->no_tracking)
			{
				if(!_current_load_results)
				{
					throw std::runtime_error("Cannot execute 'get_documents' before operation execution.");
				}
				std::for_each(_ids.cbegin(), _ids.cend(), [&](const std::string& id)
				{
					if(!id.empty())
					{
						results.insert_or_assign(id, nullptr);
					}
				});

				for(auto& doc: _current_load_results->results)
				{
					if(doc.is_null())
					{
						continue;
					}
					auto doc_info = DocumentInfo(doc);
					results.insert_or_assign(doc_info.id, _session.lock()->track_entity<T>(doc_info, from_json, to_json));
				}
			}

			for(const auto& id : _ids)
			{
				if(!id.empty())
				{
					results.insert_or_assign(id, get_document<T>(id, from_json, to_json));
				}
			}
			return results;
		}

		void set_result(std::shared_ptr<commands::GetDocumentsResult> result);
	};

	template <typename T>
	std::shared_ptr<T> LoadOperation::get_document(const std::optional<DocumentInfo::FromJsonConverter>& from_json,
	                                               const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		if (_session.lock()->no_tracking)
		{
			if (!_current_load_results)
			{
				throw std::runtime_error("Cannot execute 'get_documents' before operation execution.");
			}

			auto document = _current_load_results->results.size() > 0
				                ? _current_load_results->results.at(0)
				                : nlohmann::json(nullptr);
			if (document.is_null())
			{
				return nullptr;
			}
			auto doc_info = DocumentInfo(document);
			return _session.lock()->track_entity<T>(doc_info, from_json, to_json);
		}
		return get_document<T>(_ids[0], from_json, to_json);
	}

	template <typename T>
	std::shared_ptr<T> LoadOperation::get_document(const std::string& id,
	                                               const std::optional<DocumentInfo::FromJsonConverter>& from_json,
	                                               const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		if (id.empty())
		{
			return {};
		}
		if (_session.lock()->is_deleted(id))
		{
			return {};
		}

		if (auto doc_info = _session.lock()->_documents_by_id.find(id);
			doc_info != _session.lock()->_documents_by_id.end())
		{
			return _session.lock()->track_entity<T>(*doc_info->second, from_json, to_json);
		}

		if (auto doc_info = _session.lock()->_included_documents_by_id.find(id);
			doc_info != _session.lock()->_included_documents_by_id.end())
		{
			return _session.lock()->track_entity<T>(*doc_info->second, from_json, to_json);
		}

		return {};
	}
}
