#pragma once
#include "IMaintenanceOperation.h"
#include "IndexDefinition.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexesOperation : public operations::IMaintenanceOperation<std::vector<documents::indexes::IndexDefinition>>
	{
	private:
		const int32_t _start;
		const int32_t _page_size;

	public:
		~GetIndexesOperation() override = default;

		GetIndexesOperation(int32_t start, int32_t page_size);

		std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::IndexDefinition>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetIndexesCommand : public http::RavenCommand<std::vector<documents::indexes::IndexDefinition>>
		{
		private:
			const int32_t _start;
			const int32_t _page_size;

		public:
			~GetIndexesCommand() override = default;

			GetIndexesCommand(int32_t start, int32_t page_size);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}



