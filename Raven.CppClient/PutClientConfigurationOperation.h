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
			const std::string _configuration_json_str;

		public:
			~PutClientConfigurationCommand() override = default;

			PutClientConfigurationCommand(ClientConfiguration configuration)
				: _configuration(std::move(configuration))
				, _configuration_json_str(nlohmann::json(_configuration).dump())
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				auto curl = curl_ref.get();
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database << "/admin/configuration/client";

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_configuration_json_str);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_configuration_json_str.length());
				curl_easy_setopt(curl, CURLOPT_PUT, 1L);
				curl_ref.method = constants::methods::PUT;

				curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

				url_ref.emplace(path_builder.str());
			}
		};
	};
}
