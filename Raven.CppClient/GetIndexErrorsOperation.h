#pragma once
#include "IMaintenanceOperation.h"
#include "IndexErrors.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexErrorsOperation : public operations::IMaintenanceOperation<std::vector<documents::indexes::IndexErrors>>
	{
	private:
		const std::vector<std::string> _index_names{};

	public:
		~GetIndexErrorsOperation() override = default;

		GetIndexErrorsOperation() = default;

		explicit GetIndexErrorsOperation(std::vector<std::string> index_names);

		std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::IndexErrors>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetIndexErrorsCommand : public http::RavenCommand<std::vector<documents::indexes::IndexErrors>>
		{
		private:
			const std::vector<std::string> _index_names{};

		public:
			~GetIndexErrorsCommand() override = default;

			explicit GetIndexErrorsCommand(std::vector<std::string> index_names);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}



