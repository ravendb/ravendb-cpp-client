#pragma once
#include "IMaintenanceOperation.h"
#include "json_utils.h"
#include "IndexStats.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexesStatisticsOperation : public operations::IMaintenanceOperation<std::vector<documents::indexes::IndexStats>>
	{
	public:
		~GetIndexesStatisticsOperation() override = default;

		GetIndexesStatisticsOperation() = default;

		std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::IndexStats>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexesStatisticsCommand>();
		}

	private:
		class GetIndexesStatisticsCommand : public http::RavenCommand<std::vector<documents::indexes::IndexStats>>
		{
		public:
			~GetIndexesStatisticsCommand() override = default;

			GetIndexesStatisticsCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes/stats";

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if(!response)
				{
					throw_invalid_response();
				}

				_result = std::make_shared<ResultType>();
				if (!impl::utils::json_utils::get_val_from_json(*response, "Results", *_result))
				{
					throw_invalid_response();
				}
			}

			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}



