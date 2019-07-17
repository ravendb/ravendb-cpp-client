#pragma once
#include "IMaintenanceOperation.h"
#include "DatabaseStatistics.h"

namespace ravendb::client::documents::operations
{
	class GetStatisticsOperation : public IMaintenanceOperation<DatabaseStatistics>
	{
	private:
		const std::optional<std::string> _debug_tag{};

	public:
		~GetStatisticsOperation() override = default;

		explicit GetStatisticsOperation(std::optional<std::string> debug_tag = {});

		std::unique_ptr<http::RavenCommand<DatabaseStatistics>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetStatisticsCommand : public http::RavenCommand<DatabaseStatistics>
		{
		private:
			const std::optional<std::string> _debug_tag{};

		public:
			~GetStatisticsCommand() override = default;

			explicit GetStatisticsCommand(std::optional<std::string> debug_tag);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
