#pragma once
#include "IOperation.h"
#include "OperationIdResult.h"
#include "IndexQuery.h"
#include "QueryOperationOptions.h"
#include "utils.h"

namespace ravendb::client::documents::operations
{
	class PatchByQueryOperation : public IOperation<OperationIdResult>
	{
	private:
		const queries::IndexQuery _query_to_update;
		const queries::QueryOperationOptions _options;

	public:
		~PatchByQueryOperation() override = default;

		explicit PatchByQueryOperation(std::string query_to_update);

		explicit PatchByQueryOperation(queries::IndexQuery query_to_update,
			queries::QueryOperationOptions options = queries::QueryOperationOptions());
		
		std::unique_ptr<http::RavenCommand<OperationIdResult>> get_command(
			std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions,
			std::shared_ptr<http::HttpCache> cache) const override;

	private:
		class PatchByQueryCommand : public http::RavenCommand<OperationIdResult>
		{
		private:
			const std::shared_ptr<conventions::DocumentConventions> _conventions;//TODO currently unused, check later
			std::istringstream _query_to_update_stream;
			const queries::QueryOperationOptions _options;

		public:
			~PatchByQueryCommand() override = default;

			PatchByQueryCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
				const queries::IndexQuery& query_to_update, queries::QueryOperationOptions options);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
