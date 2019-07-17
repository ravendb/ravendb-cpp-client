#pragma once
#include "IMaintenanceOperation.h"

namespace ravendb::client::documents::operations
{
	class GetOperationStateOperation : public operations::IMaintenanceOperation<nlohmann::json>
	{
	private:
		const int64_t _id;
		
	public:
		~GetOperationStateOperation() override = default;

		explicit GetOperationStateOperation(int64_t id);

		std::unique_ptr<http::RavenCommand<nlohmann::json>> get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetOperationStateCommand : public http::RavenCommand<nlohmann::json>
		{
		private:
			const std::shared_ptr<conventions::DocumentConventions> _conventions;
			const int64_t _id;

		public:
			~GetOperationStateCommand() override = default;

			GetOperationStateCommand(std::shared_ptr<conventions::DocumentConventions> conventions, int64_t id);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
