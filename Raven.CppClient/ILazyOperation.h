#pragma once

namespace ravendb::client::documents
{
	namespace queries
	{
		struct QueryResult;
	}
	namespace commands::multi_get
	{
		struct GetRequest;
		struct GetResponse;
	}
}

namespace ravendb::client::documents::session::operations::lazy
{
	struct ILazyOperation
	{
		virtual ~ILazyOperation() = 0;

		virtual std::optional<commands::multi_get::GetRequest> create_request() = 0;

		virtual queries::QueryResult get_query_result() const = 0;

		virtual bool is_requires_retry() const = 0;

		virtual void handle_response(const commands::multi_get::GetResponse& response) = 0;
	};

	inline ILazyOperation::~ILazyOperation() = default;
}
