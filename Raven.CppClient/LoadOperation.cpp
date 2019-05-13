#include "stdafx.h"
#include "LoadOperation.h"

namespace ravendb::client::documents::session::operations
{
	LoadOperation::LoadOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session) 
		: _session(session)
	{}

	std::unique_ptr<http::RavenCommand<commands::GetDocumentsResult>> LoadOperation::create_request() const
	{
		if (_ids_to_check_on_server.empty())
		{
			return {};
		}

		if (_session.lock()->check_if_already_included(_ids, _includes))
		{
			return {};
		}

		_session.lock()->increment_request_count();

		//TODO take care of counters

		return std::make_unique<commands::GetDocumentsCommand>(_ids_to_check_on_server, _includes, false);
	}

	LoadOperation& LoadOperation::with_includes(std::vector<std::string> includes)
	{
		_includes = std::move(includes);

		return *this;
	}

	LoadOperation& LoadOperation::by_id(const std::string& id)
	{
		if (_ids.empty())
		{
			_ids.push_back(id);
		}
		if (_session.lock()->is_loaded_or_deleted(id))
		{
			return *this;
		}

		_ids_to_check_on_server.push_back(id);
		return *this;
	}

	LoadOperation& LoadOperation::by_ids(const std::vector<std::reference_wrapper<const std::string>>& ids)
	{
		auto comparator = [](std::reference_wrapper<const std::string> str1,
		                     std::reference_wrapper<const std::string> str2)
		{
			return impl::utils::CompareStringsLessThanIgnoreCase()(str1.get(), str2.get());
		};

		std::transform(ids.cbegin(), ids.cend(), std::back_inserter(_ids),
		               [](const std::reference_wrapper<const std::string>& id)
		               {
			               return id.get();
		               });

		std::set<std::reference_wrapper<const std::string>, decltype(comparator)> distinct(
			ids.cbegin(), ids.cend(), comparator);
		const std::string empty_str{};
		distinct.erase(empty_str);

		std::for_each(distinct.cbegin(), distinct.cend(), [this](const std::reference_wrapper<const std::string>& id)
		{
			by_id(id.get());
		});

		return *this;
	}

	void LoadOperation::set_result(std::shared_ptr<commands::GetDocumentsResult> result)
	{
		if (!result)
		{
			return;
		}

		if (_session.lock()->no_tracking)
		{
			_current_load_results = result;
		}

		_session.lock()->register_includes(result->includes);

		//TODO take care of counters

		for (const auto& document : result->results)
		{
			if (document.is_null())
			{
				continue;
			}

			auto new_doc_info = std::make_shared<DocumentInfo>(document);
			_session.lock()->_documents_by_id.insert({new_doc_info->id, new_doc_info});
		}

		_session.lock()->register_missing_includes(result->results, result->includes, _includes);
	}
}
