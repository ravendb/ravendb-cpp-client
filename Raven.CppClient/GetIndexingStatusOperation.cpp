#include "stdafx.h"
#include "GetIndexingStatusOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	std::unique_ptr<http::RavenCommand<documents::indexes::IndexingStatus>> GetIndexingStatusOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetIndexingStatusCommand>();
	}

	void GetIndexingStatusOperation::GetIndexingStatusCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes/status";

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetIndexingStatusOperation::GetIndexingStatusCommand::set_response(
		const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool GetIndexingStatusOperation::GetIndexingStatusCommand::is_read_request() const
	{
		return true;
	}
}
