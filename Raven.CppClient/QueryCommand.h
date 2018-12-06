#pragma once
#include "stdafx.h"
#include "QueryResult.h"
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "IndexQuery.h"

namespace ravendb::client::documents::commands
{
	using
		ravendb::client::http::RavenCommand,
		ravendb::client::documents::queries::QueryResult,
		ravendb::client::documents::conventions::DocumentConventions,
		ravendb::client::documents::queries::IndexQuery;

	class QueryCommand : public RavenCommand<QueryResult>
	{
	private:
		DocumentConventions _conventions;
		IndexQuery _index_query;
		bool _metadata_only = false;
		bool _index_entries_only = false;

	public:
		~QueryCommand() = default;

		QueryCommand(DocumentConventions conventions, IndexQuery index_query, bool metadata_only, bool index_entries_only)
			: _conventions(std::move(conventions))
			, _index_query(std::move(index_query))
			, _metadata_only(metadata_only)
			, _index_entries_only(index_entries_only)
		{}

		void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override
		{
			_can_cache = !_index_query.disable_caching;

			// we won't allow aggressive caching of queries with WaitForNonStaleResults
			_can_cache_aggressively = _can_cache && !_index_query.wait_for_non_stale_results;

			std::ostringstream pathBuilder;
			pathBuilder << node.url << "/databases/" << node.database
				<< "/queries?";// << "queryHash=" << _index_query.get_query_hash;
		
			if(_metadata_only)
			{
				pathBuilder << "&metadataOnly=true";
			}
			if(_index_entries_only)
			{
				pathBuilder << "&debug=entries";
			}

			curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);

			auto&& json_str = nlohmann::json(_index_query).dump();

			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());

			url = pathBuilder.str();
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			_result = response;
		}
		
		bool is_read_request() const noexcept override
		{
			return true;
		}

	};
}
