#pragma once
#include "IMaintenanceOperation.h"
#include "IndexingStatus.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexingStatusOperation : public operations::IMaintenanceOperation<documents::indexes::IndexingStatus>
	{
	public:
		~GetIndexingStatusOperation() override = default;

		GetIndexingStatusOperation() = default;

		std::unique_ptr<http::RavenCommand<documents::indexes::IndexingStatus>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexingStatusCommand>();
		}

	private:
		class GetIndexingStatusCommand : public http::RavenCommand<documents::indexes::IndexingStatus>
		{
		public:
			~GetIndexingStatusCommand() override = default;

			GetIndexingStatusCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes/status";

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
				_result = std::make_shared<ResultType>(response->get<ResultType>());
			}

			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}



