#pragma once
#include "RavenCommand.h"
#include "IServerOperation.h"
#include "ClientConfiguration.h"

namespace ravendb::client::serverwide::operations::configuration
{
	class GetServerWideClientConfigurationOperation : public IServerOperation<documents::operations::configuration::ClientConfiguration>
	{
	public:
		~GetServerWideClientConfigurationOperation() override;

		GetServerWideClientConfigurationOperation();

		std::unique_ptr<http::RavenCommand<documents::operations::configuration::ClientConfiguration>> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class GetServerWideClientConfigurationCommand : public http::RavenCommand<documents::operations::configuration::ClientConfiguration>
		{
		public:
			~GetServerWideClientConfigurationCommand() override;

			GetServerWideClientConfigurationCommand();

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
