#pragma once
#include "IMaintenanceOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetTermsOperation : public operations::IMaintenanceOperation<std::vector<std::string>>
	{
	private:
		const std::string _index_name;
		const std::string _field;
		const std::string _from_value;
		const std::optional<int32_t> _page_size;

	public:
		~GetTermsOperation() override = default;

		GetTermsOperation(std::string index_name, std::string field, std::string from_value = {},
			std::optional<int32_t> page_size = {});

		std::unique_ptr<http::RavenCommand<std::vector<std::string>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetTermsCommand : public http::RavenCommand<std::vector<std::string>>
		{
		private:
			const std::string _index_name;
			const std::string _field;
			const std::string _from_value;
			const std::optional<int32_t> _page_size;

		public:
			~GetTermsCommand() override = default;

			GetTermsCommand(std::string index_name, std::string field, std::string from_value,
				std::optional<int32_t> page_size);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}



