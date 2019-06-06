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

		explicit PatchByQueryOperation(std::string query_to_update)
			: PatchByQueryOperation(queries::IndexQuery(std::move(query_to_update)))
		{}

		explicit PatchByQueryOperation(queries::IndexQuery query_to_update, 
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
		
		std::unique_ptr<http::RavenCommand<OperationIdResult>> get_command
		(std::shared_ptr<IDocumentStore> store, std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache) const override
		{
			return std::make_unique<PatchByQueryCommand>(conventions, _query_to_update, _options);
		}

	private:
		class PatchByQueryCommand : public http::RavenCommand<OperationIdResult>
		{
		private:
			const std::shared_ptr<conventions::DocumentConventions> _conventions;//TODO currently unused, check later
			const std::string _query_to_update_str;
			const queries::QueryOperationOptions _options;

		public:
			~PatchByQueryCommand() override = default;

			PatchByQueryCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
				const queries::IndexQuery& query_to_update, queries::QueryOperationOptions options)
				: _conventions(conventions)
				, _query_to_update_str([&]
			{
				return nlohmann::json{ {"Query",query_to_update} }.dump();
			}())
				, _options(std::move(options))
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				auto curl = curl_ref.get();
				std::ostringstream path_builder;

				path_builder << node->url << "/databases/" << node->database <<
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
				
				curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_query_to_update_str);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_query_to_update_str.length());
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
				curl_ref.method = constants::methods::PATCH;

				curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if(!response)
				{
					throw_invalid_response();
				}
				_result = std::make_shared<ResultType>(response->get<ResultType>());
			}

			bool is_read_request() const override
			{
				return false;
			}
		};
	};
}
