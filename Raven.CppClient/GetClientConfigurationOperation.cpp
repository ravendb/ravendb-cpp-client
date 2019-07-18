#include "stdafx.h"
#include "GetClientConfigurationOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::configuration
{
	void get_client_configuration_operation::from_json(const nlohmann::json& j, Result& r)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Etag", r.etag);
		get_val_from_json(j, "Configuration", r.configuration);
	}

	std::unique_ptr<http::RavenCommand<get_client_configuration_operation::Result>> 
		GetClientConfigurationOperation::get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetClientConfigurationCommand>();
	}

	void GetClientConfigurationOperation::GetClientConfigurationCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database << "/configuration/client";

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetClientConfigurationOperation::GetClientConfigurationCommand::set_response(
		const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (response)
		{
			_result = std::make_shared<ResultType>(response->get<ResultType>());
		}
	}

	bool GetClientConfigurationOperation::GetClientConfigurationCommand::is_read_request() const
	{
		return false;
	}
}
