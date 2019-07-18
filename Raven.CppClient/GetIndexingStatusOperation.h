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
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetIndexingStatusCommand : public http::RavenCommand<documents::indexes::IndexingStatus>
		{
		public:
			~GetIndexingStatusCommand() override = default;

			GetIndexingStatusCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}



