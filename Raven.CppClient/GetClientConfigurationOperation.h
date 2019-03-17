#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "json_utils.h"

namespace get_client_configuration_operation
{
	struct Result
	{
		int64_t etag{};
		std::optional<ravendb::client::documents::operations::configuration::ClientConfiguration> configuration{};
	};

	inline void from_json(const nlohmann::json& j, Result& r)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Etag", r.etag);
		get_val_from_json(j, "Configuration", r.configuration);
	}
}
namespace ravendb::client::documents::operations::configuration
{
	class GetClientConfigurationOperation : public 
		operations::IMaintenanceOperation<get_client_configuration_operation::Result>
	{
	public:
		~GetClientConfigurationOperation() override = default;

		GetClientConfigurationOperation() = default;

		std::unique_ptr<RavenCommand<get_client_configuration_operation::Result>> 
			get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<GetClientConfigurationCommand>();
		}

	private:
		class GetClientConfigurationCommand : public RavenCommand<get_client_configuration_operation::Result>
		{
		public:
			~GetClientConfigurationCommand() override = default;

			GetClientConfigurationCommand() = default;

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database << "/configuration/client";

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if (!response.is_null())
				{
					_result = response;
				}
			}

			bool is_read_request() const noexcept override
			{
				return false;
			}
		};
	};
}
