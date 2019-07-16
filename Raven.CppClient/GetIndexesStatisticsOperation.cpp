#include "stdafx.h"
#include "GetIndexesStatisticsOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::indexes
{
	std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::IndexStats>>> GetIndexesStatisticsOperation::
		get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetIndexesStatisticsCommand>();
	}

	void GetIndexesStatisticsOperation::GetIndexesStatisticsCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes/stats";

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetIndexesStatisticsOperation::GetIndexesStatisticsCommand::set_response(
		const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}

		_result = std::make_shared<ResultType>();
		if (!impl::utils::json_utils::get_val_from_json(*response, "Results", *_result))
		{
			throw_invalid_response();
		}
	}

	bool GetIndexesStatisticsOperation::GetIndexesStatisticsCommand::is_read_request() const
	{
		return true;
	}
}
