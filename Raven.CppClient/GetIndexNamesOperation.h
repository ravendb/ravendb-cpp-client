#pragma once
#include "IMaintenanceOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexNamesOperation : public operations::IMaintenanceOperation<std::vector<std::string>>
	{
	private:
		const int32_t _start;
		const int32_t _page_size;

	public:
		~GetIndexNamesOperation() override = default;

		GetIndexNamesOperation(int32_t start, int32_t page_size);

		std::unique_ptr<http::RavenCommand<std::vector<std::string>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetIndexNamesCommand : public http::RavenCommand<std::vector<std::string>>
		{
		private:
			const int32_t _start;
			const int32_t _page_size;

		public:
			~GetIndexNamesCommand() override = default;

			GetIndexNamesCommand(int32_t start, int32_t page_size);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;


			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
