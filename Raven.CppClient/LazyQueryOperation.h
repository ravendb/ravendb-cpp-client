#pragma once
#include "ILazyOperation.h"
#include "QueryOperation.h"
#include "GetRequest.h"
#include "GetResponse.h"
#include "IndexQueryContent.h"

namespace ravendb::client::documents::session::operations::lazy
{
	template<typename T>
	class LazyQueryOperation : public ILazyOperation
	{
	public:
		using ResultType = std::vector<std::shared_ptr<T>>;

	private:
		std::shared_ptr<conventions::DocumentConventions> _conventions;
		std::shared_ptr<QueryOperation> _query_operation;
		const std::vector<std::function<void(const queries::QueryResult&)>> _after_query_executed;

		ResultType _result{};
		queries::QueryResult _query_result{};
		bool _requires_retry = false;

		void handle_response(std::shared_ptr<const queries::QueryResult> query_result);

	public:
		~LazyQueryOperation() override = default;

		LazyQueryOperation(std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<QueryOperation> query_operation,
			std::vector<std::function<void(const queries::QueryResult&)>> after_query_executed);

		std::optional<commands::multi_get::GetRequest> create_request() override;

		ResultType get_result() const;

		queries::QueryResult get_query_result() const override;

		bool is_requires_retry() const override;

		void handle_response(const commands::multi_get::GetResponse& response) override;
	};


	template <typename T>
	void LazyQueryOperation<T>::handle_response(std::shared_ptr<const queries::QueryResult> query_result)
	{
		if (!query_result)
		{
			return;
		}
		_query_operation->ensure_is_acceptable_and_save_result(query_result);
		for(const auto& event : _after_query_executed)
		{
			event(*query_result);
		}
		_result = _query_operation->complete<T>();
		_query_result = *query_result;
	}

	template <typename T>
	LazyQueryOperation<T>::LazyQueryOperation(std::shared_ptr<conventions::DocumentConventions> conventions,
		std::shared_ptr<QueryOperation> query_operation,
		std::vector<std::function<void(const queries::QueryResult&)>> after_query_executed)
		: _conventions(conventions)
		, _query_operation(query_operation)
		, _after_query_executed(std::move(after_query_executed))
	{}

	template <typename T>
	std::optional<commands::multi_get::GetRequest> LazyQueryOperation<T>::create_request()
	{
		auto request = std::optional<commands::multi_get::GetRequest>(std::in_place);

		request->url = "/queries";
		request->method = "POST";
		request->query = "?queryHash=" + _query_operation->get_index_query().get_query_hash();
		request->content = std::make_unique<IndexQueryContent>(_conventions, _query_operation->get_index_query());

		return std::move(request);
	}

	template <typename T>
	typename LazyQueryOperation<T>::ResultType LazyQueryOperation<T>::get_result() const
	{
		return _result;
	}

	template <typename T>
	queries::QueryResult LazyQueryOperation<T>::get_query_result() const
	{
		return _query_result;
	}

	template <typename T>
	bool LazyQueryOperation<T>::is_requires_retry() const
	{
		return _requires_retry;
	}

	template <typename T>
	void LazyQueryOperation<T>::handle_response(const commands::multi_get::GetResponse& response)
	{
		if (response.force_retry)
		{
			_result.clear();
			_requires_retry = true;
			return;
		}

		std::shared_ptr<queries::QueryResult> query_result{};
		if (response.result)
		{
			query_result = std::make_shared<queries::QueryResult>(nlohmann::json::parse(*response.result).get<queries::QueryResult>());
		}
		handle_response(query_result);
	}
}
