#include "stdafx.h"
#include "GetCollectionStatisticsOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	GetCollectionStatisticsOperation::~GetCollectionStatisticsOperation() = default;

	GetCollectionStatisticsOperation::GetCollectionStatisticsOperation() = default;

	std::unique_ptr<http::RavenCommand<CollectionStatistics>> GetCollectionStatisticsOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetCollectionStatisticsCommand>();
	}

	GetCollectionStatisticsOperation::GetCollectionStatisticsCommand::~GetCollectionStatisticsCommand() = default;

	GetCollectionStatisticsOperation::GetCollectionStatisticsCommand::GetCollectionStatisticsCommand() = default;

	void GetCollectionStatisticsOperation::GetCollectionStatisticsCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database << "/collections/stats";

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetCollectionStatisticsOperation::GetCollectionStatisticsCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}

		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool GetCollectionStatisticsOperation::GetCollectionStatisticsCommand::is_read_request() const
	{
		return true;
	}
}
