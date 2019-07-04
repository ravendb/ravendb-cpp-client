#pragma once
#include "IVoidServerOperation.h"
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "VoidRavenCommand.h"

namespace ravendb::client::http
{
	struct ServerNode;
}

namespace ravendb::client::serverwide::operations::configuration
{
	class PutServerWideClientConfigurationOperation : public IVoidServerOperation
	{
	private:
		const documents::operations::configuration::ClientConfiguration _configuration;

	public:
		~PutServerWideClientConfigurationOperation() override;

		explicit PutServerWideClientConfigurationOperation(documents::operations::configuration::ClientConfiguration configuration);

		std::unique_ptr<http::RavenCommand<void>> get_command(std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class PutServerWideClientConfigurationCommand : public http::VoidRavenCommand
		{
		private:
			std::istringstream _configuration_document_stream;

		public:
			~PutServerWideClientConfigurationCommand() override;

			PutServerWideClientConfigurationCommand(std::shared_ptr<documents::conventions::DocumentConventions> conventions,
				const documents::operations::configuration::ClientConfiguration& configuration);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}
