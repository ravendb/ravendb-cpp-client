#pragma once
#include "IMaintenanceOperation.h"
#include "IndexStats.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexesStatisticsOperation : public operations::IMaintenanceOperation<std::vector<documents::indexes::IndexStats>>
	{
	public:
		~GetIndexesStatisticsOperation() override = default;

		GetIndexesStatisticsOperation() = default;

		std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::IndexStats>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetIndexesStatisticsCommand : public http::RavenCommand<std::vector<documents::indexes::IndexStats>>
		{
		public:
			~GetIndexesStatisticsCommand() override = default;

			GetIndexesStatisticsCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}



