#pragma once
#include "IMaintenanceOperation.h"
#include "PutIndexResult.h"
#include "IndexDefinition.h"

namespace ravendb::client::documents::operations::indexes
{
	class PutIndexesOperation : public IMaintenanceOperation<std::vector<documents::indexes::PutIndexResult>>
	{
	private:
		const std::vector<documents::indexes::IndexDefinition> _index_to_add;

	public:
		~PutIndexesOperation() override = default;

		explicit PutIndexesOperation(std::vector<documents::indexes::IndexDefinition> index_to_add);
		
		std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::PutIndexResult>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class PutIndexesCommand : public http::RavenCommand<std::vector<documents::indexes::PutIndexResult>>
		{
		private:
			const std::vector<nlohmann::json> _index_to_add;
			std::istringstream _index_to_add_stream;

		public:
			~PutIndexesCommand() override = default;

			explicit PutIndexesCommand(const std::vector<documents::indexes::IndexDefinition>& indexes_to_add);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}



