#pragma once
#include "IOperation.h"
#include "OperationIdResult.h"
#include "IndexQuery.h"
#include "QueryOperationOptions.h"

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

		std::unique_ptr<http::RavenCommand<OperationIdResult>> get_command
		(std::shared_ptr<IDocumentStore> store, std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache) const override
		{
			return std::make_unique<DeleteByQueryCommand>(conventions, _query_to_delete, _options);
		}

	private:
		class DeleteByQueryCommand : public http::RavenCommand<OperationIdResult>
		{
		private:
			const std::shared_ptr<conventions::DocumentConventions> _conventions;//TODO currently unused, check later
			const std::string _query_to_delete_str;
			const std::optional<queries::QueryOperationOptions> _options;

		public:
			~DeleteByQueryCommand() override = default;

			DeleteByQueryCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
				const queries::IndexQuery& query_to_delete, std::optional<queries::QueryOperationOptions> options)
				: _conventions(conventions)
				, _query_to_delete_str(nlohmann::json(query_to_delete).dump())
				, _options(std::move(options))
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				auto curl = curl_ref.get();
				std::ostringstream path_builder;

				path_builder << node->url << "/databases/" << node->database <<
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
				
				curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_query_to_delete_str);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_query_to_delete_str.length());
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
				curl_ref.method = constants::methods::DELETE_;

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
