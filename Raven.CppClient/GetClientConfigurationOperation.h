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

		std::unique_ptr<http::RavenCommand<get_client_configuration_operation::Result>> 
			get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetClientConfigurationCommand>();
		}

	private:
		class GetClientConfigurationCommand : public http::RavenCommand<get_client_configuration_operation::Result>
		{
		public:
			~GetClientConfigurationCommand() override = default;

			GetClientConfigurationCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database << "/configuration/client";

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if (response)
				{
					_result = std::make_shared<ResultType>(response->get<ResultType>());
				}
			}

			bool is_read_request() const override
			{
				return false;
			}
		};
	};
}
