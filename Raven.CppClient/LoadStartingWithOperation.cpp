#include "stdafx.h"
#include "LoadStartingWithOperation.h"

namespace ravendb::client::documents::session::operations
{
	LoadStartingWithOperation::LoadStartingWithOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session)
		: _session(session)
	{}

	std::unique_ptr<http::RavenCommand<commands::GetDocumentsResult>> LoadStartingWithOperation::create_request() const
	{
		_session.lock()->increment_request_count();

		//TODO logger
		//if (logger.isInfoEnabled()) {
		//	logger.info("Requesting documents with ids starting with '" + _startWith + "' from " + _session.storeIdentifier());
		//}

		return std::make_unique<commands::GetDocumentsCommand>(
			_start_with, _start_after, _matches, _exclude, _start, _page_size, false);
	}

	void LoadStartingWithOperation::with_start_with(std::string id_prefix,
		std::optional<std::string> matches,
		int32_t start,
		int32_t page_size,
		std::optional<std::string> exclude,
		std::optional<std::string> start_after)
	{
		_start_with = std::move(id_prefix);
		_matches = std::move(matches);
		_start = start;
		_page_size = page_size;
		_exclude = std::move(exclude);
		_start_after = std::move(start_after);
	}

	void LoadStartingWithOperation::set_result(std::shared_ptr<commands::GetDocumentsResult> result)
	{
		if(_session.lock()->no_tracking)
		{
			_current_load_results = result;
			return;
		}

		for(const auto& document : result->results)
		{
			if (document.is_null())
			{
				continue;
			}

			auto new_doc_info = std::make_shared<DocumentInfo>(document);
			_session.lock()->_documents_by_id.insert({ new_doc_info->id, new_doc_info });
			_returned_ids.push_back(new_doc_info->id);
		}
	}
}
