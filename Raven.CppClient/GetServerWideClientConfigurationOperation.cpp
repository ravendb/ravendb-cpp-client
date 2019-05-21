#include "stdafx.h"
#include "GetServerWideClientConfigurationOperation.h"

namespace ravendb::client::serverwide::operations::configuration
{
	GetServerWideClientConfigurationOperation::~GetServerWideClientConfigurationOperation() = default;

	GetServerWideClientConfigurationOperation::GetServerWideClientConfigurationOperation() = default;

	std::unique_ptr<http::RavenCommand<documents::operations::configuration::ClientConfiguration>>
	GetServerWideClientConfigurationOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return  std::make_unique<GetServerWideClientConfigurationCommand>();
	}

	GetServerWideClientConfigurationOperation::GetServerWideClientConfigurationCommand::~GetServerWideClientConfigurationCommand() = default;

	GetServerWideClientConfigurationOperation::GetServerWideClientConfigurationCommand::GetServerWideClientConfigurationCommand() = default;

	void GetServerWideClientConfigurationOperation::GetServerWideClientConfigurationCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/configuration/client";

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetServerWideClientConfigurationOperation::GetServerWideClientConfigurationCommand::set_response(
		const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if(!response)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool GetServerWideClientConfigurationOperation::GetServerWideClientConfigurationCommand::is_read_request() const
	{
		return false;
	}
}
