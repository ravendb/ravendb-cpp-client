#pragma once
#include "stdafx.h"
#include "QueryResult.h"
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "IndexQuery.h"

namespace ravendb::client::documents::commands
{
	class QueryCommand : public http::RavenCommand<queries::QueryResult>
	{
	private:
		const std::shared_ptr<conventions::DocumentConventions> _conventions;
		queries::IndexQuery _index_query;
		bool _metadata_only = false;
		bool _index_entries_only = false;

	public:
		~QueryCommand() override = default;

		QueryCommand(std::shared_ptr<conventions::DocumentConventions> conventions, queries::IndexQuery index_query,
			bool metadata_only, bool index_entries_only)
			: _conventions(conventions)
			, _index_query(std::move(index_query))
			, _metadata_only(metadata_only)
			, _index_entries_only(index_entries_only)
		{}

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override
		{
			_can_cache = !_index_query.disable_caching;

			// we won't allow aggressive caching of queries with WaitForNonStaleResults
			_can_cache_aggressively = _can_cache && !_index_query.wait_for_non_stale_results;

			auto curl = curl_ref.get();
			std::ostringstream path_builder;

			path_builder << node->url << "/databases/" << node->database
				<< "/queries?";// TODO << "queryHash=" << _index_query.get_query_hash;
		
			if(_metadata_only)
			{
				path_builder << "&metadataOnly=true";
			}
			if(_index_entries_only)
			{
				path_builder << "&debug=entries";
			}

			curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
			curl_ref.method = constants::methods::POST;

			auto&& json_str = nlohmann::json(_index_query).dump();

			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());

			curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

			url_ref.emplace(path_builder.str());
		}

		void  set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
		{
			if(!response)
			{
				_result.reset();
				return;
			}
			_result = std::make_shared<ResultType>(response->get<ResultType>());
			//TODO
			//if (fromCache) {
			//	result.setDurationInMs(-1);

			//	if (result.getTimings() != null) {
			//		result.getTimings().setDurationInMs(-1);
			//		result.setTimings(null);
			//	}
			//}
		}
		
		bool is_read_request() const override
		{
			return true;
		}

	};
}
