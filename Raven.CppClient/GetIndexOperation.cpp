#include "stdafx.h"
#include "GetIndexOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::indexes
{
	GetIndexOperation::GetIndexOperation(std::string index_name)
		: _index_name(std::move(index_name))
	{}

	std::unique_ptr<http::RavenCommand<documents::indexes::IndexDefinition>> GetIndexOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetIndexCommand>(_index_name);
	}

	GetIndexOperation::GetIndexCommand::GetIndexCommand(std::string index_name)
		: _index_name(std::move(index_name))
	{}

	void GetIndexOperation::GetIndexCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes?name=" << http::url_encode(curl_ref, _index_name);

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetIndexOperation::GetIndexCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		if (!response)
		{
			return;
		}
		if (std::vector<documents::indexes::IndexDefinition> results{};
			!impl::utils::json_utils::get_val_from_json(*response, "Results", results) ||
			results.size() != 1)
		{
			throw_invalid_response();
		}
		else
		{
			_result = std::make_shared<ResultType>(std::move(results[0]));
		}
	}

	bool GetIndexOperation::GetIndexCommand::is_read_request() const
	{
		return true;
	}
}
