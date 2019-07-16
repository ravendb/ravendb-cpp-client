#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "json.hpp"

namespace ravendb::client::documents::operations::configuration
{
	namespace get_client_configuration_operation
	{
		struct Result
		{
			int64_t etag{};
			std::optional<ClientConfiguration> configuration{};
		};

		void from_json(const nlohmann::json& j, Result& r);
	}


	class GetClientConfigurationOperation : public 
		operations::IMaintenanceOperation<get_client_configuration_operation::Result>
	{
	public:
		~GetClientConfigurationOperation() override = default;

		GetClientConfigurationOperation() = default;

		std::unique_ptr<http::RavenCommand<get_client_configuration_operation::Result>>
			get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetClientConfigurationCommand : public http::RavenCommand<get_client_configuration_operation::Result>
		{
		public:
			~GetClientConfigurationCommand() override = default;

			GetClientConfigurationCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
