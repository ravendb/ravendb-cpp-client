#pragma once
#include "IMaintenanceOperation.h"
#include "IndexDefinition.h"

namespace ravendb::client::documents::operations::indexes
{
	class IndexHasChangedOperation : public IMaintenanceOperation<bool>
	{
	private:
		const documents::indexes::IndexDefinition _index_definition;

	public:
		~IndexHasChangedOperation() override = default;

		explicit IndexHasChangedOperation(documents::indexes::IndexDefinition definition);

		std::unique_ptr<http::RavenCommand<bool>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class IndexHasChangedCommand : public http::RavenCommand<bool>
		{
		private:
			nlohmann::json _definition;

		public:
			~IndexHasChangedCommand() override = default;

			IndexHasChangedCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
				const documents::indexes::IndexDefinition& definition);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}

