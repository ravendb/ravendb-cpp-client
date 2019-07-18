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

		template <typename ConstIterator>
		static uint64_t calculate_docs_ids_hash(ConstIterator begin, ConstIterator end);

		void prepare_request_with_multiple_ids(std::ostringstream& path_builder,
			impl::CurlHandlesHolder::CurlReference& curl_ref);

	public:
		~GetDocumentsCommand() override = default;

		GetDocumentsCommand(int32_t start, int32_t pageSize);

		GetDocumentsCommand(std::string id, std::vector<std::string> includes, bool metadata_only);

		GetDocumentsCommand(const std::vector<std::string>& ids, std::vector<std::string> includes, bool metadata_only);

		//TODO counters
		//public GetDocumentsCommand(String[] ids, String[] includes, String[] counterIncludes, boolean metadataOnly) {
		//	this(ids, includes, metadataOnly);

		//	if (counterIncludes == null) {
		//		throw new IllegalArgumentException("CounterIncludes cannot be null");
		//	}

		//	_counters = counterIncludes;
		//}

		GetDocumentsCommand(
			std::string start_with,
			std::optional<std::string> start_after,
			std::optional<std::string> matches,
			std::optional<std::string> exclude
			, int32_t start
			, int32_t pageSize
			, bool metadataOnly);

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

		bool is_read_request() const override;
	};

	//using xxhash_cpp from https://github.com/RedSpah/xxhash_cpp
	//TODO consider the endian issue
	template <typename ConstIterator>
	uint64_t GetDocumentsCommand::calculate_docs_ids_hash(ConstIterator begin, ConstIterator end)
	{
		xxh::hash_state_t<64> hash_stream;
		for (ConstIterator it = begin; it != end; ++it)
		{
			hash_stream.update(*it);
		}

		return hash_stream.digest();
	}
}
