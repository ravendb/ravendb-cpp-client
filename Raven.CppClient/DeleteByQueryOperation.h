#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "IOperation.h"
#include "OperationIdResult.h"
#include "IndexQuery.h"
#include "QueryOperationOptions.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode;

namespace ravendb::client::documents::operations
{
	class DeleteByQueryOperation : public IOperation<OperationIdResult>
	{
	private:
		const queries::IndexQuery _query_to_delete;
		const std::optional<queries::QueryOperationOptions> _options;

	public:
		~DeleteByQueryOperation() override = default;


		explicit DeleteByQueryOperation
			(queries::IndexQuery query_to_delete, std::optional<queries::QueryOperationOptions> options = {})
			: _query_to_delete(std::move(query_to_delete))
			, _options(std::move(options))
		{}

		std::unique_ptr<RavenCommand<OperationIdResult>> get_command
		(std::shared_ptr<IDocumentStore> store, std::shared_ptr<DocumentConventions> conventions, HttpCache& cache) const override
		{
			return std::make_unique<DeleteByQueryCommand>(conventions, _query_to_delete, _options);
		}

	private:
		class DeleteByQueryCommand : public RavenCommand<OperationIdResult>
		{
		private:
			const std::shared_ptr<DocumentConventions> _conventions;//TODO currently unused, check later
			const std::string _query_to_delete_str;
			const std::optional<queries::QueryOperationOptions> _options;

		public:
			~DeleteByQueryCommand() override = default;

			DeleteByQueryCommand(std::shared_ptr<DocumentConventions> conventions,
				const queries::IndexQuery& query_to_delete, std::optional<queries::QueryOperationOptions> options)
				: _conventions(conventions)
				, _query_to_delete_str(nlohmann::json(query_to_delete).dump())
				, _options(std::move(options))
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database <<
					"/queries" <<
					"?allowStale=";
				if(_options)
				{
					path_builder << std::boolalpha << _options->allow_stale;
				}

				if(_options && _options->max_ops_per_second)
				{
					path_builder << "&maxOpsPerSec=" << *_options->max_ops_per_second;
				}

				path_builder << "&details=";
				if (_options)
				{
					path_builder << std::boolalpha << _options->retrieve_details;
				}

				if (_options && _options->stale_timeout)
				{
					path_builder << "&staleTimeout=" << impl::utils::MillisToTimeSpanConverter(*_options->stale_timeout);
				}
				
				curl_easy_setopt(curl, CURLOPT_READFUNCTION, impl::utils::read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_query_to_delete_str);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_query_to_delete_str.length());
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

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
