#pragma once
#include "CollectionStatistics.h"
#include "IMaintenanceOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetCollectionStatisticsOperation : public IMaintenanceOperation<CollectionStatistics>
	{
	public:
		~GetCollectionStatisticsOperation() override;

		GetCollectionStatisticsOperation();

		std::unique_ptr<http::RavenCommand<CollectionStatistics>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetCollectionStatisticsCommand : public http::RavenCommand<CollectionStatistics>
		{
		public:
			~GetCollectionStatisticsCommand() override;

			GetCollectionStatisticsCommand();

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override;

			bool is_read_request() const noexcept override;
		};
	};
}
