#include "stdafx.h"
#include "GetIndexStatisticsOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::indexes
{
	GetIndexStatisticsOperation::GetIndexStatisticsOperation(std::string index_name)
		: _index_name(std::move(index_name))
	{}

	std::unique_ptr<http::RavenCommand<documents::indexes::IndexStats>> GetIndexStatisticsOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetIndexStatisticsCommand>(_index_name);
	}

	GetIndexStatisticsOperation::GetIndexStatisticsCommand::GetIndexStatisticsCommand(std::string index_name)
		: _index_name(std::move(index_name))
	{}

	void GetIndexStatisticsOperation::GetIndexStatisticsCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes/stats?name=" << http::url_encode(curl_ref, _index_name);

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetIndexStatisticsOperation::GetIndexStatisticsCommand::set_response(
		const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}

		std::vector<documents::indexes::IndexStats> stats{};
		if (!impl::utils::json_utils::get_val_from_json(*response, "Results", stats) ||
			stats.size() != 1)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>(stats[0]);
	}

	bool GetIndexStatisticsOperation::GetIndexStatisticsCommand::is_read_request() const
	{
		return true;
	}
}
