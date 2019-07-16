#pragma once
#include "IVoidMaintenanceOperation.h"
#include "ClientConfiguration.h"

namespace ravendb::client::documents::operations::configuration
{
	class PutClientConfigurationOperation : public IVoidMaintenanceOperation
	{
	private:
		const ClientConfiguration _configuration;

	public:
		~PutClientConfigurationOperation() override = default;

		explicit PutClientConfigurationOperation(ClientConfiguration configuration);

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class PutClientConfigurationCommand : public http::VoidRavenCommand
		{
		private:
			const ClientConfiguration _configuration;
			std::istringstream _configuration_json_stream;

		public:
			~PutClientConfigurationCommand() override = default;

			explicit PutClientConfigurationCommand(ClientConfiguration configuration);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}
