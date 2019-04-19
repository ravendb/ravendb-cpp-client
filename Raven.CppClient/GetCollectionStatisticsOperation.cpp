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

	void GetCollectionStatisticsOperation::GetCollectionStatisticsCommand::create_request(CURL* curl,
		const http::ServerNode& node, std::string& url)
	{
		std::ostringstream path_builder;
		path_builder << node.url << "/databases/" << node.database << "/collections/stats";

		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

		url = path_builder.str();
	}

	void GetCollectionStatisticsOperation::GetCollectionStatisticsCommand::set_response(CURL* curl,
		const nlohmann::json& response, bool from_cache)
	{
		if (response.is_null())
		{
			throw_invalid_response();
		}

		_result = std::make_shared<ResultType>(response.get<ResultType>());
	}

	bool GetCollectionStatisticsOperation::GetCollectionStatisticsCommand::is_read_request() const noexcept
	{
		return true;
	}
}
