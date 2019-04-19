#pragma once
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "json_utils.h"
#include "IndexStats.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexesStatisticsOperation : public operations::IMaintenanceOperation<std::vector<documents::indexes::IndexStats>>
	{
	public:
		~GetIndexesStatisticsOperation() override = default;

		GetIndexesStatisticsOperation() = default;

		std::unique_ptr<RavenCommand<std::vector<documents::indexes::IndexStats>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexesStatisticsCommand>();
		}

	private:
		class GetIndexesStatisticsCommand : public RavenCommand<std::vector<documents::indexes::IndexStats>>
		{
		public:
			~GetIndexesStatisticsCommand() override = default;

			GetIndexesStatisticsCommand() = default;

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database
					<< "/indexes/stats";

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				_result = std::make_shared<ResultType>();
				if (!impl::utils::json_utils::get_val_from_json(response, "Results", *_result))
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::INVALID_RESPONSE);
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}



