#include "stdafx.h"
#include "QueryCommand.h"

namespace ravendb::client::documents::commands
{
	QueryCommand::QueryCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
		queries::IndexQuery index_query, bool metadata_only, bool index_entries_only)
		: _conventions(conventions)
		, _index_query(std::move(index_query))
		, _metadata_only(metadata_only)
		, _index_entries_only(index_entries_only)
	{}

	void QueryCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		_can_cache = !_index_query.disable_caching;

		// we won't allow aggressive caching of queries with WaitForNonStaleResults
		_can_cache_aggressively = _can_cache && !_index_query.wait_for_non_stale_results;

		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database
			<< "/queries?"
			// we need to add a query hash because we are using POST queries
			// so we need to unique parameter per query so the query cache will
			// work properly
			<< "queryHash=" << _index_query.get_query_hash();

		if (_metadata_only)
		{
			path_builder << "&metadataOnly=true";
		}
		if (_index_entries_only)
		{
			path_builder << "&debug=entries";
		}

		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_ref.method = constants::methods::POST;

		auto&& json_str = nlohmann::json(_index_query).dump();

		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(json_str.size()));
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());

		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		url_ref.emplace(path_builder.str());
	}

	void QueryCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			_result.reset();
			return;
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());

		if(from_cache)
		{
			_result->duration_in_ms = -1;

			if(_result->timings)
			{
				_result->timings->duration_in_ms = -1;//TODO probably redundant
				_result->timings.reset();
			}
		}
	}

	bool QueryCommand::is_read_request() const
	{
		return true;
	}
}
