#pragma once
#include "IMaintenanceOperation.h"
#include "IndexDefinition.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexOperation : public operations::IMaintenanceOperation<documents::indexes::IndexDefinition>
	{
	private:
		const std::string _index_name;

	public:
		~GetIndexOperation() override = default;

		explicit GetIndexOperation(std::string index_name);

		std::unique_ptr<http::RavenCommand<documents::indexes::IndexDefinition>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class GetIndexCommand : public http::RavenCommand<documents::indexes::IndexDefinition>
		{
		private:
			const std::string _index_name;

		public:
			~GetIndexCommand() override = default;

			explicit GetIndexCommand(std::string index_name);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}



