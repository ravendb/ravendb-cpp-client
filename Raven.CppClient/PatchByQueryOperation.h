#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "IOperation.h"
#include "OperationIdResult.h"
#include "IndexQuery.h"
#include "QueryOperationOptions.h"
#include "utils.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode,
ravendb::client::documents::queries::IndexQuery;

namespace ravendb::client::documents::operations
{
	class PatchByQueryOperation : public IOperation<OperationIdResult>
	{
	private:
		const IndexQuery _query_to_update;
		const queries::QueryOperationOptions _options;

	public:
		~PatchByQueryOperation() override = default;

		explicit PatchByQueryOperation(std::string query_to_update)
			: PatchByQueryOperation(IndexQuery(std::move(query_to_update)))
		{}

		explicit PatchByQueryOperation(IndexQuery query_to_update, 
			queries::QueryOperationOptions options = queries::QueryOperationOptions())
			: _query_to_update([&]
		{
			if (impl::utils::is_blank(query_to_update.query))
				throw std::invalid_argument("Query must have a non empty value");
			else
				return std::move(query_to_update);
		}())
			, _options(std::move(options))
		{}
		
		std::unique_ptr<RavenCommand<OperationIdResult>> get_command
		(std::shared_ptr<IDocumentStore> store, std::shared_ptr<DocumentConventions> conventions, HttpCache& cache) const override
		{
			return std::make_unique<PatchByQueryCommand>(conventions, _query_to_update, _options);
		}

	private:
		class PatchByQueryCommand : public RavenCommand<OperationIdResult>
		{
		private:
			const std::shared_ptr<DocumentConventions> _conventions;//TODO currently unused, check later
			const std::string _query_to_update_str;
			const queries::QueryOperationOptions _options;

		public:
			~PatchByQueryCommand() override = default;

			PatchByQueryCommand(std::shared_ptr<DocumentConventions> conventions,
				const queries::IndexQuery& query_to_update, queries::QueryOperationOptions options)
				: _conventions(conventions)
				, _query_to_update_str([&]
			{
				return nlohmann::json{ {"Query",query_to_update} }.dump();
			}())
				, _options(std::move(options))
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database <<
					"/queries?allowStale=" << std::boolalpha << _options.allow_stale;

				if (_options.max_ops_per_second)
				{
					path_builder << "&maxOpsPerSec=" << _options.max_ops_per_second.value();
				}
				path_builder << "&details=" << std::boolalpha << _options.retrieve_details;
				if (_options.stale_timeout)
				{
					path_builder << "&staleTimeout=" << impl::utils::MillisToTimeSpanConverter(*_options.stale_timeout);
				}
				
				curl_easy_setopt(curl, CURLOPT_READFUNCTION, impl::utils::read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_query_to_update_str);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_query_to_update_str.length());
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				_result = std::make_shared<ResultType>(response.get<ResultType>());
			}

			bool is_read_request() const noexcept override
			{
				return false;
			}
		};
	};
}
