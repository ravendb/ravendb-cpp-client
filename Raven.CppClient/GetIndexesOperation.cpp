#include "stdafx.h"
#include "GetIndexesOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::indexes
{
	GetIndexesOperation::GetIndexesOperation(int32_t start, int32_t page_size)
		: _start(start)
		, _page_size(page_size)
	{}

	std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::IndexDefinition>>> GetIndexesOperation::
		get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetIndexesCommand>(_start, _page_size);
	}

	GetIndexesOperation::GetIndexesCommand::GetIndexesCommand(int32_t start, int32_t page_size)
		: _start(start)
		, _page_size(page_size)
	{}

	void GetIndexesOperation::GetIndexesCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes?start=" << _start << "&pageSize=" << _page_size;

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetIndexesOperation::GetIndexesCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
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

	bool GetIndexesOperation::GetIndexesCommand::is_read_request() const
	{
		return true;
	}
}
