#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "GetDocumentsCommand.h"

namespace ravendb::client::documents::session::operations
{
	class LoadStartingWithOperation
	{
	private:
		//TODO private static final Log logger = LogFactory.getLog(LoadStartingWithOperation.class);

		const std::weak_ptr<InMemoryDocumentSessionOperations> _session;

		std::string _start_with{};
		std::optional<std::string> _matches{};
		int32_t _start{};
		int32_t _page_size{};
		std::optional<std::string> _exclude{};
		std::optional<std::string> _start_after{};

		std::vector<std::string> _returned_ids{};
		std::shared_ptr<commands::GetDocumentsResult> _current_load_results{};

	public:
		LoadStartingWithOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session);

		std::unique_ptr<http::RavenCommand<commands::GetDocumentsResult>> create_request() const;

		void with_start_with(std::string id_prefix,
			std::optional<std::string> matches = {},
			int32_t start = 0,
			int32_t page_size = 25,
			std::optional<std::string> exclude = {},
			std::optional<std::string> start_after = {});

		void set_result(std::shared_ptr<commands::GetDocumentsResult> result);

		template<typename T>
		std::vector<std::shared_ptr<T>> get_documents(
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		std::shared_ptr<T> get_document(
			const std::string& id,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});
	};

	template <typename T>
	std::vector<std::shared_ptr<T>> LoadStartingWithOperation::get_documents(
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		std::vector<std::shared_ptr<T>> final_results{};

		if (_session.lock()->no_tracking)
		{
			if (!_current_load_results)
			{
				throw std::runtime_error("Cannot execute 'get_documents' before operation execution.");
			}

			final_results.reserve(_current_load_results->results.size());

			for (auto& document : _current_load_results->results)
			{
				auto doc_info = DocumentInfo(document);
				final_results.push_back(_session.lock()->track_entity<T>(doc_info, from_json, to_json));
			}
		}
		else
		{
			final_results.reserve(_returned_ids.size());
			for (const auto& id : _returned_ids)
			{
				final_results.push_back(get_document<T>(id));
			}
		}

		return final_results;
	}

	template <typename T>
	std::shared_ptr<T> LoadStartingWithOperation::get_document(const std::string& id,
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

		return {};
	}
}
