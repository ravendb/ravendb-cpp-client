#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::documents::operations::configuration
{
	class PutClientConfigurationOperation : public IVoidMaintenanceOperation
	{
	private:
		const ClientConfiguration _configuration;

	public:
		~PutClientConfigurationOperation() override = default;

		PutClientConfigurationOperation(ClientConfiguration configuration)
			: _configuration(std::move(configuration))
		{}

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<PutClientConfigurationCommand>(_configuration);
		}

	private:
		class PutClientConfigurationCommand : public http::VoidRavenCommand
		{
		private:
			const ClientConfiguration _configuration;
			std::istringstream _configuration_json_stream;

		public:
			~PutClientConfigurationCommand() override = default;

			PutClientConfigurationCommand(ClientConfiguration configuration)
				: _configuration(std::move(configuration))
				, _configuration_json_stream(nlohmann::json(_configuration).dump())
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				auto curl = curl_ref.get();
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database << "/admin/configuration/client";

				curl_ref.method = constants::methods::PUT;
				curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
				curl_ref.headers.emplace(constants::headers::CONTENT_TYPE, "application/json");

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_configuration_json_stream);

				_configuration_json_stream.clear();
				_configuration_json_stream.seekg(0);

				url_ref.emplace(path_builder.str());
			}
		};
	};
}
