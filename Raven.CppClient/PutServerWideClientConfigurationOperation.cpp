#include "stdafx.h"
#include "PutServerWideClientConfigurationOperation.h"

namespace ravendb::client::serverwide::operations::configuration
{
	PutServerWideClientConfigurationOperation::~PutServerWideClientConfigurationOperation() = default;

	PutServerWideClientConfigurationOperation::PutServerWideClientConfigurationOperation(
		documents::operations::configuration::ClientConfiguration configuration)
		: _configuration(std::move(configuration))
	{}

	std::unique_ptr<http::RavenCommand<void>> PutServerWideClientConfigurationOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<PutServerWideClientConfigurationCommand>(conventions, _configuration);
	}

	PutServerWideClientConfigurationOperation::PutServerWideClientConfigurationCommand::
		~PutServerWideClientConfigurationCommand() = default;

	PutServerWideClientConfigurationOperation::PutServerWideClientConfigurationCommand::
	PutServerWideClientConfigurationCommand(std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		const documents::operations::configuration::ClientConfiguration& configuration)
		: _configuration_document_stream(nlohmann::json(configuration).dump())
	{}

	void PutServerWideClientConfigurationOperation::PutServerWideClientConfigurationCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/admin/configuration/client";

		curl_ref.method = constants::methods::PUT;
		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.headers.emplace(constants::headers::CONTENT_TYPE, "application/json");

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_configuration_document_stream);

		_configuration_document_stream.clear();
		_configuration_document_stream.seekg(0);

		url_ref.emplace(path_builder.str());
	}
}
