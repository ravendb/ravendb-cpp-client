#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "GetDocumentsResult.h"
#include "GetDocumentsCommand.h"

namespace ravendb::client::documents::session::operations
{
	class LoadOperation
	{
	private:
		const std::reference_wrapper<InMemoryDocumentSessionOperations> _session;

		std::vector<std::string> _ids{};

		std::vector<std::string> _ids_to_check_on_server{};

		std::optional<commands::GetDocumentsResult> _current_load_results{};

	public:
		LoadOperation(InMemoryDocumentSessionOperations& session)
			: _session(std::ref(session))
		{}

		std::unique_ptr<RavenCommand<commands::GetDocumentsResult>> create_request()
		{
			if(_ids_to_check_on_server.empty())
			{
				return {};
			}

			_session.get().increment_request_count();

			return std::make_unique<commands::GetDocumentsCommand>(_ids_to_check_on_server, std::vector<std::string>(), false);
		}

		LoadOperation& by_id(const std::string& id)
		{
			if(_ids.empty())
			{
				_ids.push_back(id);
			}
			if(_session.get().is_loaded_or_deleted(id))
			{
				return *this;
			}

			_ids_to_check_on_server.push_back(id);
			return *this;
		}

		template<typename T>
		std::shared_ptr<T> get_document()
		{
			if(_session.get().no_tracking)
			{
				//TODO complete
			}

			return get_document<T>(_ids[0]);//TODO check if _ids[0] is OK
		}
		template<typename T>
		std::shared_ptr<T> get_document(const std::string& id)
		{
			if(id.empty())
			{
				return {};
			}
			if(_session.get().is_deleted(id))
			{
				return {};
			}
			{
				auto doc_info = _session.get()._documents_by_id.find(id);
				if (doc_info != _session.get()._documents_by_id.end())
				{
					return _session.get().track_entity<T>(*doc_info->second);
				}
			}
			{
				auto doc_info = _session.get()._included_documents_by_id.find(id);
				if (doc_info != _session.get()._included_documents_by_id.end())
				{
					return _session.get().track_entity<T>(*doc_info->second);
				}
			}
			return {};
		}

		void set_result(const commands::GetDocumentsResult& result)
		{
			if(_session.get().no_tracking)
			{
				_current_load_results = result;
			}

			//TODO add later _session.get().register_includes(result.includes);

			for(const auto& document : result.results)
			{
				if(document.empty() || document.is_null())
				{
					continue;
				}

				auto new_doc_info = std::make_shared<DocumentInfo>(document);
				_session.get()._documents_by_id.insert({new_doc_info->id,new_doc_info});
			}

			//TODO add later _session.get().register_missing_includes(result.results, result.includes, _includes);
		}
	};
}
