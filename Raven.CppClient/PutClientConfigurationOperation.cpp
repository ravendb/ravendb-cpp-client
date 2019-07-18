#include "stdafx.h"
#include "PutClientConfigurationOperation.h"

namespace ravendb::client::documents::operations::configuration
{
	PutClientConfigurationOperation::PutClientConfigurationOperation(ClientConfiguration configuration)
		: _configuration(configuration)
	{}

	std::unique_ptr<http::VoidRavenCommandBase> PutClientConfigurationOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<PutClientConfigurationCommand>(_configuration);
	}

	PutClientConfigurationOperation::PutClientConfigurationCommand::PutClientConfigurationCommand(
		ClientConfiguration configuration)
		: _configuration(configuration)
		, _configuration_json_stream(nlohmann::json(_configuration).dump())
	{}

	void PutClientConfigurationOperation::PutClientConfigurationCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database << "/admin/configuration/client";

		curl_ref.method = constants::methods::PUT;
		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_configuration_json_stream);

		_configuration_json_stream.clear();
		_configuration_json_stream.seekg(0);

		url_ref.emplace(path_builder.str());
	}
}
