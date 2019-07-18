#include "stdafx.h"
#include "GetIndexErrorsOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	GetIndexErrorsOperation::GetIndexErrorsOperation(std::vector<std::string> index_names)
		: _index_names(std::move(index_names))
	{}

	std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::IndexErrors>>> GetIndexErrorsOperation::
		get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetIndexErrorsCommand>(_index_names);
	}

	GetIndexErrorsOperation::GetIndexErrorsCommand::GetIndexErrorsCommand(std::vector<std::string> index_names)
		: _index_names(std::move(index_names))
	{}

	void GetIndexErrorsOperation::GetIndexErrorsCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database << "/indexes/errors";

		if (!_index_names.empty())
		{
			path_builder << "?";
			for (const auto& index_name : _index_names)
			{
				path_builder << "&name=" << index_name;
			}
		}

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetIndexErrorsOperation::GetIndexErrorsCommand::set_response(const std::optional<nlohmann::json>& response,
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

	bool GetIndexErrorsOperation::GetIndexErrorsCommand::is_read_request() const
	{
		return true;
	}
}
