#include "stdafx.h"
#include "GetBuildNumberOperation.h"

namespace ravendb::client::serverwide::operations
{
	std::unique_ptr<http::RavenCommand<BuildNumber>> GetBuildNumberOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetBuildNumberCommand>();
	}

	void GetBuildNumberOperation::GetBuildNumberCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream url_builder;
		url_builder << node->url << "/build/version";

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(url_builder.str());
	}

	void GetBuildNumberOperation::GetBuildNumberCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool GetBuildNumberOperation::GetBuildNumberCommand::is_read_request() const
	{
		return true;
	}
}
