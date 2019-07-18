#include "stdafx.h"
#include "PatchByQueryOperation.h"

namespace ravendb::client::documents::operations
{
	PatchByQueryOperation::PatchByQueryOperation(std::string query_to_update)
		: PatchByQueryOperation(queries::IndexQuery(std::move(query_to_update)))
	{}

	PatchByQueryOperation::PatchByQueryOperation(queries::IndexQuery query_to_update,
		queries::QueryOperationOptions options)
		: _query_to_update([&]
	{
		if (impl::utils::is_blank(query_to_update.query))
			throw std::invalid_argument("Query must have a non empty value");
		else
			return std::move(query_to_update);
	}())
		, _options(options)
	{}

	std::unique_ptr<http::RavenCommand<OperationIdResult>> PatchByQueryOperation::get_command(
		std::shared_ptr<IDocumentStore> store, std::shared_ptr<conventions::DocumentConventions> conventions,
		std::shared_ptr<http::HttpCache> cache) const
	{
		return std::make_unique<PatchByQueryCommand>(conventions, _query_to_update, _options);
	}

	PatchByQueryOperation::PatchByQueryCommand::PatchByQueryCommand(
		std::shared_ptr<conventions::DocumentConventions> conventions, const queries::IndexQuery& query_to_update,
		queries::QueryOperationOptions options)
		: _conventions(conventions)
		, _query_to_update_stream([&]
	{
		return nlohmann::json{ {"Query",query_to_update} }.dump();
	}())
		, _options(options)
	{}

	void PatchByQueryOperation::PatchByQueryCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database <<
			"/queries?allowStale=" << std::boolalpha << _options.allow_stale;

		if (_options.max_ops_per_second)
		{
			path_builder << "&maxOpsPerSec=" << _options.max_ops_per_second.value();
		}
		path_builder << "&details=" << std::boolalpha << _options.retrieve_details;
		if (_options.stale_timeout)
		{
			path_builder << "&staleTimeout=" << impl::utils::MillisToTimeSpanConverter(*_options.stale_timeout);
		}

		curl_ref.method = constants::methods::PATCH;
		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_query_to_update_stream);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");

		_query_to_update_stream.clear();
		_query_to_update_stream.seekg(0);

		url_ref.emplace(path_builder.str());
	}

	void PatchByQueryOperation::PatchByQueryCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool PatchByQueryOperation::PatchByQueryCommand::is_read_request() const
	{
		return false;
	}
}
