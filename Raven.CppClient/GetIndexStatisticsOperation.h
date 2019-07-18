#pragma once
#include "IMaintenanceOperation.h"
#include "IndexStats.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexStatisticsOperation : public operations::IMaintenanceOperation<documents::indexes::IndexStats>
	{
	private:
		const std::string _index_name;

	public:
		~GetIndexStatisticsOperation() override = default;

		explicit GetIndexStatisticsOperation(std::string index_name);

		std::unique_ptr<http::RavenCommand<documents::indexes::IndexStats>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetIndexStatisticsCommand : public http::RavenCommand<documents::indexes::IndexStats>
		{
		private:
			const std::string _index_name;

		public:
			~GetIndexStatisticsCommand() override = default;

			explicit GetIndexStatisticsCommand(std::string index_name);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
