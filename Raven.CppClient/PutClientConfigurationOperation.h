#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IVoidMaintenanceOperation.h"
#include "utils.h"

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

		std::unique_ptr<VoidRavenCommandBase> get_command(std::shared_ptr<DocumentConventions> conventions) const override
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

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database << "/admin/configuration/client";

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, ravendb::client::impl::utils::read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_PUT, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_configuration_json_str);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_configuration_json_str.length());

				_headers_list.append("Content-Type: application/json");
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list.get());

				url = path_builder.str();
			}
		};
	};
}
