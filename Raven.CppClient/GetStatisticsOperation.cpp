#include "stdafx.h"
#include "GetStatisticsOperation.h"

namespace ravendb::client::documents::operations
{
	GetStatisticsOperation::GetStatisticsOperation(std::optional<std::string> debug_tag)
		: _debug_tag(std::move(debug_tag))
	{}

	std::unique_ptr<http::RavenCommand<DatabaseStatistics>> GetStatisticsOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetStatisticsCommand>(_debug_tag);
	}

	GetStatisticsOperation::GetStatisticsCommand::GetStatisticsCommand(std::optional<std::string> debug_tag)
		: _debug_tag(std::move(debug_tag))
	{}

	void GetStatisticsOperation::GetStatisticsCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database << "/stats";

		if (_debug_tag)
		{
			path_builder << "?" << *_debug_tag;
		}

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetStatisticsOperation::GetStatisticsCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool GetStatisticsOperation::GetStatisticsCommand::is_read_request() const
	{
		return true;
	}
}
