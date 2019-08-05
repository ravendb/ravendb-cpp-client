#include "stdafx.h"
#include "GetOperationStateOperation.h"

namespace ravendb::client::documents::operations
{
	GetOperationStateOperation::GetOperationStateOperation(int64_t id)
		: _id(id)
	{}

	std::unique_ptr<http::RavenCommand<nlohmann::json>> GetOperationStateOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetOperationStateCommand>(conventions::DocumentConventions::default_conventions(), _id);
	}

	GetOperationStateOperation::GetOperationStateCommand::GetOperationStateCommand(
		std::shared_ptr<conventions::DocumentConventions> conventions, int64_t id)
		: _conventions(conventions)
		, _id(id)
	{}

	void GetOperationStateOperation::GetOperationStateCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/operations/state?id=" << _id;

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetOperationStateOperation::GetOperationStateCommand::set_response(
		const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			return;
		}
		_result = std::make_shared<ResultType>(response.value());
	}

	bool GetOperationStateOperation::GetOperationStateCommand::is_read_request() const
	{
		return true;
	}
}
