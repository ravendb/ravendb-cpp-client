#include "stdafx.h"
#include "GetServerWideOperationStateOperation.h"
#include "DocumentConventions.h"

namespace ravendb::client::serverwide::operations
{
	GetServerWideOperationStateOperation::GetServerWideOperationStateOperation(int64_t id)
		: _id(id)
	{}

	std::unique_ptr<http::RavenCommand<nlohmann::json>> GetServerWideOperationStateOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetServerWideOperationStateCommand>(documents::conventions::DocumentConventions::default_conventions(), _id);
	}

	GetServerWideOperationStateOperation::GetServerWideOperationStateCommand::GetServerWideOperationStateCommand(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions, int64_t id)
		: _id(id)
		, _conventions(conventions)
	{}

	void GetServerWideOperationStateOperation::GetServerWideOperationStateCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;

		path_builder << node->url << "/operations/state?id=" << _id;

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetServerWideOperationStateOperation::GetServerWideOperationStateCommand::set_response(
		const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if(!response)
		{
			return;
		}

		_result = std::make_shared<ResultType>(*response);
	}

	bool GetServerWideOperationStateOperation::GetServerWideOperationStateCommand::is_read_request() const
	{
		return true;
	}
}
