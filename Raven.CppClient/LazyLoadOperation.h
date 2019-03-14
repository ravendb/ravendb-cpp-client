#pragma once
#include "ILazyOperation.h"
#include "InMemoryDocumentSessionOperations.h"
#include "LoadOperation.h"
#include "GetRequest.h"
#include "GetResponse.h"

namespace ravendb::client::documents::session::operations::lazy
{
	template<typename T>
	class LazyLoadOperation : public ILazyOperation
	{
	public:
		using ResultType = DocumentsByIdsMap<T>;

	private:
		const std::shared_ptr<InMemoryDocumentSessionOperations> _session;
		std::unique_ptr<LoadOperation> _load_operation;
		std::vector<std::string> _ids{};
		std::vector<std::string> _includes{};

		ResultType _result{};
		QueryResult _query_result{};
		boolean _requires_retry = false;

		void handle_response(const std::optional<commands::GetDocumentsResult>& load_result);

	public:
		~LazyLoadOperation() override = default;

		LazyLoadOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::unique_ptr<LoadOperation> load_operation);

		std::optional<commands::multi_get::GetRequest> create_request() override;

		LazyLoadOperation<T>& by_id(std::string id);

		LazyLoadOperation<T>& by_ids(std::vector<std::string> ids);

		LazyLoadOperation<T>& with_includes(std::vector<std::string> includes);

		ResultType get_result() const;

		QueryResult get_query_result() const override;

		bool is_requires_retry() const override;

		void handle_response(const commands::multi_get::GetResponse& response) override;
	};


	template <typename T>
	void LazyLoadOperation<T>::handle_response(const std::optional<commands::GetDocumentsResult>& load_result)
	{
		if (load_result)
		{
			_load_operation->set_result(*load_result);
		}

		if (!_requires_retry)
		{
			_result = ResultType(_load_operation->get_documents<T>());
		}
	}

	template <typename T>
	LazyLoadOperation<T>::LazyLoadOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::unique_ptr<LoadOperation> load_operation)
		: _session(session)
		, _load_operation(std::move(load_operation))
	{}

	template <typename T>
	std::optional<commands::multi_get::GetRequest> LazyLoadOperation<T>::create_request()
	{
		std::vector<std::reference_wrapper<const std::string>> ids_to_check_on_server{};
		ids_to_check_on_server.reserve(_ids.size());
		std::for_each(_ids.cbegin(), _ids.cend(), [&](const std::string& id)
		{
			if (!_session->is_loaded_or_deleted(id))
			{
				ids_to_check_on_server.push_back(std::cref(id));
			}
		});

		std::ostringstream query_builder{};
		query_builder << "?";
		std::for_each(_includes.cbegin(), _includes.cend(), [&](const std::string& include)
		{
			query_builder << "&include=" << include;
		});

		std::for_each(ids_to_check_on_server.cbegin(), ids_to_check_on_server.cend(),
			[&](const std::reference_wrapper<const std::string>& id)
		{
		  query_builder << "&id=" << impl::utils::url_escape(nullptr, id.get());
		});

		if (const bool has_items = !ids_to_check_on_server.empty();
			!has_items)
		{
			// no need to hit the server
			_result = ResultType(_load_operation->get_documents<T>());
			return {};
		}

		auto get_request = std::optional<commands::multi_get::GetRequest>(std::in_place);
		get_request->url = "/docs";
		get_request->query = query_builder.str();
		return std::move(get_request);
	}

	template <typename T>
	LazyLoadOperation<T>& LazyLoadOperation<T>::by_id(std::string id)
	{
		if (_ids.empty())
		{
			_ids.push_back(std::move(id));
		}
		return *this;
	}

	template <typename T>
	LazyLoadOperation<T>& LazyLoadOperation<T>::by_ids(std::vector<std::string> ids)
	{
		_ids = std::move(ids);

		return *this;
	}

	template <typename T>
	LazyLoadOperation<T>& LazyLoadOperation<T>::with_includes(std::vector<std::string> includes)
	{
		_includes = std::move(includes);

		return *this;
	}

	template <typename T>
	typename LazyLoadOperation<T>::ResultType LazyLoadOperation<T>::get_result() const
	{
		return _result;
	}

	template <typename T>
	queries::QueryResult LazyLoadOperation<T>::get_query_result() const
	{
		return _query_result;
	}

	template <typename T>
	bool LazyLoadOperation<T>::is_requires_retry() const
	{
		return _requires_retry;
	}

	template <typename T>
	void LazyLoadOperation<T>::handle_response(const commands::multi_get::GetResponse& response)
	{
		if (response.force_retry)
		{
			_result.clear();
			_requires_retry = true;
			return;
		}

		std::optional<commands::GetDocumentsResult> multi_load_result{};
		if (response.result)
		{
			multi_load_result = nlohmann::json::parse(*response.result).get<commands::GetDocumentsResult>();
		}
		handle_response(multi_load_result);
	}
}
