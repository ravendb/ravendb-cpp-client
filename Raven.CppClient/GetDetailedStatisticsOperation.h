#pragma once
#include "IMaintenanceOperation.h"
#include "DetailedDatabaseStatistics.h"

namespace ravendb::client::documents::operations
{
	class GetDetailedStatisticsOperation : public IMaintenanceOperation<DetailedDatabaseStatistics>
	{
	private:
		const std::optional<std::string> _debug_tag;

	public:
		~GetDetailedStatisticsOperation() override;

		explicit GetDetailedStatisticsOperation(std::optional<std::string> debug_tag = {});

		std::unique_ptr<http::RavenCommand<DetailedDatabaseStatistics>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class DetailedDatabaseStatisticsCommand : public http::RavenCommand<DetailedDatabaseStatistics>
		{
		private:
			const std::optional<std::string> _debug_tag;

		public:
			~DetailedDatabaseStatisticsCommand() override;

			explicit DetailedDatabaseStatisticsCommand(std::optional<std::string> debug_tag = {});

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
