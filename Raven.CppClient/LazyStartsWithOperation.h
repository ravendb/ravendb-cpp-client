#pragma once
#include "ILazyOperation.h"
#include "InMemoryDocumentSessionOperations.h"
#include "GetRequest.h"
#include "GetResponse.h"
#include "QueryResult.h"

namespace ravendb::client::documents::session::operations::lazy
{
	template<typename T>
	class LazyStartsWithOperation : public ILazyOperation
	{
	public:
		using ResultType = DocumentsByIdsMap<T>;

	private:
		const std::weak_ptr<InMemoryDocumentSessionOperations> _session;

		std::string _id_prefix{};
		std::optional<std::string> _matches{};
		int32_t _start{};
		int32_t _page_size{};
		std::optional<std::string> _exclude{};
		std::optional<std::string> _start_after{};

		ResultType _result{};
		queries::QueryResult _query_result{};
		bool _requires_retry = false;

		const std::optional<DocumentInfo::FromJsonConverter> from_json;
		const std::optional<DocumentInfo::ToJsonConverter> to_json;

	public:
		~LazyStartsWithOperation() override = default;

		LazyStartsWithOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session,
			std::string id_prefix,
			std::optional<std::string> matches = {},
			int32_t start = 0,
			int32_t page_size = 25,
			std::optional<std::string> exclude = {},
			std::optional<std::string> start_after = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		std::optional<commands::multi_get::GetRequest> create_request() override;

		ResultType get_result() const;

		queries::QueryResult get_query_result() const override;

		bool is_requires_retry() const override;

		void handle_response(const commands::multi_get::GetResponse& response) override;
	};

	template <typename T>
	LazyStartsWithOperation<T>::LazyStartsWithOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::string id_prefix, std::optional<std::string> matches, int32_t start, int32_t page_size,
		std::optional<std::string> exclude, std::optional<std::string> start_after,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
		: _session(session)
		, _id_prefix(std::move(id_prefix))
		, _matches(std::move(matches))
		, _start(start)
		, _page_size(page_size)
		, _exclude(std::move(exclude))
		, _start_after(std::move(start_after))
		, from_json(from_json)
		, to_json(to_json)
	{}


	template <typename T>
	std::optional<commands::multi_get::GetRequest> LazyStartsWithOperation<T>::create_request()
	{
		std::ostringstream query_builder{};
		std::string empty_string{};
		query_builder <<
			"?startsWith=" << http::url_encode(_id_prefix) <<
			"&matches=" << (_matches ? *_matches : empty_string) <<
			"&exclude=" << (_exclude ? *_exclude : empty_string) <<
			"&start=" << _start <<
			"&pageSize=" << _page_size <<
			"&startAfter=" << (_start_after ? *_start_after : empty_string);

		auto get_request = std::optional<commands::multi_get::GetRequest>(std::in_place);
		get_request->url = "/docs";
		get_request->query = query_builder.str();
		return std::move(get_request);
	}


	template <typename T>
	typename LazyStartsWithOperation<T>::ResultType LazyStartsWithOperation<T>::get_result() const
	{
		return _result;
	}

	template <typename T>
	queries::QueryResult LazyStartsWithOperation<T>::get_query_result() const
	{
		return _query_result;
	}

	template <typename T>
	bool LazyStartsWithOperation<T>::is_requires_retry() const
	{
		return _requires_retry;
	}

	template <typename T>
	void LazyStartsWithOperation<T>::handle_response(const commands::multi_get::GetResponse& response)
	{
		//TODO keep it ?
		//if (response.force_retry)
		//{
		//	_result.clear();
		//	_requires_retry = true;
		//	return;
		//}

		auto multi_load_result = std::make_shared<commands::GetDocumentsResult>();
		if (response.result)
		{
			*multi_load_result = nlohmann::json::parse(*response.result).get<commands::GetDocumentsResult>();
		}

		auto final_results = ResultType();

		for(const auto& document : multi_load_result->results)
		{
			auto new_doc_info = std::make_shared<DocumentInfo>(document);
			_session.lock()->_documents_by_id.insert_or_assign(new_doc_info->id, new_doc_info);

			if(_session.lock()->is_deleted(new_doc_info->id))
			{
				final_results.insert_or_assign(new_doc_info->id, nullptr);
				continue;
			}

			if(auto doc_info_it = _session.lock()->_documents_by_id.find(new_doc_info->id);
				doc_info_it != _session.lock()->_documents_by_id.end())
			{
				final_results.insert_or_assign(new_doc_info->id, _session.lock()->
					track_entity<T>(*(doc_info_it->second), from_json, to_json));
				continue;
			}

			final_results.insert_or_assign(new_doc_info->id, nullptr);
		}

		_result = std::move(final_results);
	}
}
