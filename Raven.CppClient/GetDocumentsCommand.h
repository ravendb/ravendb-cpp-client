#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "GetDocumentsResult.h"
#include "xxhash.hpp"

namespace ravendb::client::documents::commands
{
	class GetDocumentsCommand : public http::RavenCommand<GetDocumentsResult>
	{
	private:
		std::optional<std::string> _id;

		std::vector<std::string> _ids{};
		std::vector<std::string> _includes{};
		//TODO counters
		//private String[] _counters;
		//private boolean _includeAllCounters;

		bool _metadata_only = false;

		std::optional<std::string> _start_with{};
		std::optional<std::string> _matches{};
		std::optional<int32_t> _start{};
		std::optional<int32_t> _page_size{};
		std::optional<std::string> _exclude{};
		std::optional<std::string> _start_after{};

		//using xxhash_cpp from https://github.com/RedSpah/xxhash_cpp
		//TODO consider the endian issue
		template <typename ConstIterator>
		static uint64_t calculate_docs_ids_hash(ConstIterator begin, ConstIterator end)
		{
			xxh::hash_state_t<64> hash_stream;
			for(ConstIterator it = begin; it != end; ++it)
			{
				hash_stream.update(*it);
			}

			return hash_stream.digest();
		}

		void prepare_request_with_multiple_ids(std::ostringstream& path_builder,
			impl::CurlHandlesHolder::CurlReference& curl_ref)
		{
			std::size_t totalLen = 0;
			const auto& unique_ids = _ids;

			std::for_each(unique_ids.cbegin(), unique_ids.cend(), [&](const std::string& id) {totalLen += id.length(); });

			bool isGet = totalLen < 1024;
			// if it is too big, we drop to POST (note that means that we can't use the HTTP cache any longer)
			// we are fine with that, requests to load > 1024 items are going to be rare

			if (isGet)// can use GET
			{
				for (const auto& id : unique_ids)
				{
					path_builder << "&id=" << http::url_encode(curl_ref, id);
				}
				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;
				return;
			}
			else // ids too big, must use POST
			{
				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPPOST, 1);
				curl_ref.method = constants::methods::POST;

				uint64_t hash = calculate_docs_ids_hash(unique_ids.cbegin(), unique_ids.cend());
				path_builder << "&loadHash=" << hash;				

				auto&& json_str = nlohmann::json({ {"Ids", unique_ids} }).dump();
				curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, json_str.c_str());

				curl_ref.headers.emplace(constants::headers::CONTENT_TYPE, "application/json");
			}
		}

	public:
		~GetDocumentsCommand() override = default;

		GetDocumentsCommand(int32_t start, int32_t pageSize)
			: _start(start)
			, _page_size(pageSize)
		{}

		GetDocumentsCommand(std::string id, std::vector<std::string> includes, bool _metadataOnly)
			: _id(std::move(id))
			, _includes(std::move(includes))
			, _metadata_only(_metadataOnly)
		{}

		GetDocumentsCommand(const std::vector<std::string>& ids, std::vector<std::string> includes, bool _metadataOnly)
			: _ids([](const std::vector<std::string>& ids)
		{
			if (ids.empty())
				throw std::invalid_argument("Please supply at least one id");
			std::vector <std::string> no_dupl_ids;
			for(const auto& id : ids)
			{
				if(std::find(no_dupl_ids.cbegin(),no_dupl_ids.cend(),id) == no_dupl_ids.cend())
				{
					no_dupl_ids.push_back(id);
				}
			}
			return no_dupl_ids;
		}(ids))
			, _includes(std::move(includes))
			, _metadata_only(_metadataOnly)
		{}

		//TODO counters
		//public GetDocumentsCommand(String[] ids, String[] includes, String[] counterIncludes, boolean metadataOnly) {
		//	this(ids, includes, metadataOnly);

		//	if (counterIncludes == null) {
		//		throw new IllegalArgumentException("CounterIncludes cannot be null");
		//	}

		//	_counters = counterIncludes;
		//}

		GetDocumentsCommand
			(std::string start_with,
			 std::optional<std::string> start_after,
			 std::optional<std::string> matches,
			 std::optional<std::string> exclude
			, int32_t start
			, int32_t pageSize
			, bool metadataOnly)
			: _metadata_only(metadataOnly)
			, _start_with(std::move(start_with))
			, _matches(std::move(matches))
			, _start(start)
			, _page_size(pageSize)
			, _exclude(std::move(exclude))
			, _start_after(std::move(start_after))
		{}

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override
		{
			std::ostringstream path_builder{};
			path_builder << node->url << "/databases/" << node->database << "/docs?";

			if (_start.has_value())
			{
				path_builder << "&start=" << std::to_string(_start.value());
			}

			if (_page_size.has_value())
			{
				path_builder << "&pageSize=" << std::to_string(_page_size.value());
			}

			if (_metadata_only)
			{
				path_builder << "&metadataOnly=true";
			}

			if (_start_with.has_value())
			{
				path_builder << "&startsWith=" << http::url_encode(curl_ref, *_start_with);
				if (_matches)
				{
					path_builder << "&matches=" << *_matches;
				}
				if (_exclude)
				{
					path_builder << "&exclude=" << *_exclude;
				}
				if (_start_after)
				{
					path_builder << "&startAfter=" << *_start_after;
				}
			}

			for (auto const& include : _includes)
			{
				path_builder << "&include=" << include;
			}

			//TODO counters
			//if (_includeAllCounters) {
			//	pathBuilder
			//		.append("&counter=")
			//		.append(Constants.Counters.ALL);
			//}
			//else if (_counters != null && _counters.length > 0) {
			//	for (String counter : _counters) {
			//		pathBuilder.append("&counter=").append(counter);
			//	}
			//}

			curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
			curl_ref.method = constants::methods::GET;

			if (_id)
			{
				path_builder << "&id=" << http::url_encode(curl_ref, *_id);
			}
			else if (!_ids.empty())
			{
				prepare_request_with_multiple_ids(path_builder, curl_ref);
			}

			url_ref.emplace(path_builder.str());
		}

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
		{
			if (response)
			{
				_result = std::make_shared<ResultType>(response->get<ResultType>());
			}
			else
			{
				_result.reset();
			}
		}

		bool is_read_request() const override
		{
			return true;
		}
	};
}
