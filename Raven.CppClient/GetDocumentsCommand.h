#pragma once
#include "RavenCommand.h"
#include "CurlHandlesHolder.h"
#include "types.h"

namespace ravendb
{
	class GetDocumentsCommand : public RavenCommand<GetDocumentsResult>
	{
	private:
		std::string _id;
		std::vector<std::string> _ids;
		std::vector<std::string> _includes;

		bool _metadataOnly = false;

		std::string _start_with;
		std::string _start_after;
		std::string _matches;
		std::string _exclude;
		std::optional<int32_t> _start;
		std::optional<int32_t> _pageSize;

	public:

		~GetDocumentsCommand() override = default;

		GetDocumentsCommand(int32_t start, int32_t pageSize)
			: _start(start)
			, _pageSize(pageSize)
		{}

		GetDocumentsCommand(std::string id, const std::vector<std::string>& includes, bool _metadataOnly)
			: _id(std::move(id))
			, _includes(includes.cbegin(), includes.cend())
			, _metadataOnly(_metadataOnly)
		{}

		GetDocumentsCommand(const std::vector<std::string>& ids, const std::vector<std::string>& includes, bool _metadataOnly)
			: _ids([&]
		{
			if (ids.empty())
				throw std::invalid_argument("Please supply at least one id");
			else return std::move(ids);
		}())
			, _includes(includes.cbegin(), includes.cend())
			, _metadataOnly(_metadataOnly)
		{}

		GetDocumentsCommand
		(std::string startWith
			, std::string startAfter
			, std::string matches
			, std::string exclude
			, int32_t start
			, int32_t pageSize
			, bool metadataOnly)
			: _metadataOnly(metadataOnly)
			, _start_with([&]
			{
				if (startWith.empty())
					throw std::invalid_argument("startWith cannot be empty");
				else return std::move(startWith);
			}())
			, _start_after(std::move(startAfter))
			, _matches(std::move(matches))
			, _exclude(std::move(exclude))
			, _start(start)
			, _pageSize(pageSize)

		{}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			std::ostringstream pathBuilder;
			pathBuilder << node.url << "/databases/" << node.database << "/docs?";

			if (_start.has_value())
			{
				pathBuilder << "&start=" << std::to_string(_start.value());
			}

			if (_pageSize.has_value())
			{
				pathBuilder << "&pageSize=" << std::to_string(_pageSize.value());
			}

			if (_metadataOnly)
			{
				pathBuilder << "&metadataOnly=true";
			}

			if (not _start_with.empty())
			{
				pathBuilder << "&startsWith=" << ravendb::impl::Utils::url_escape(curl, _start_with);

				if (not _matches.empty())
				{
					pathBuilder << "&matches=" << ravendb::impl::Utils::url_escape(curl, _matches);
				}

				if (not _exclude.empty())
				{
					pathBuilder << "&exclude=" << ravendb::impl::Utils::url_escape(curl, _exclude);
				}

				if (not _start_after.empty())
				{
					pathBuilder << "&startAfter=" << ravendb::impl::Utils::url_escape(curl, _start_after);
				}
			}

			for (auto const& include : _includes)
			{
				pathBuilder << "&include=" << include;
			}

			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			if (not _id.empty())
			{
				pathBuilder << "&id=" << ravendb::impl::Utils::url_escape(curl, _id);
			}
			else if (not _ids.empty())
			{
				prepareRequestWithMultipleIds(pathBuilder, curl, _ids);
			}

			url = pathBuilder.str();
		}

		static void prepareRequestWithMultipleIds(std::ostringstream& pathBuilder, CURL* curl, const std::vector<std::string> ids)
		{
			std::size_t totalLen = 0;
			auto uniqueIds = std::set<std::string>(ids.cbegin(), ids.cend());

			std::for_each(uniqueIds.cbegin(), uniqueIds.cend(), [&](std::string id) {totalLen += id.length(); });

			bool isGet = totalLen < 1024;
			// if it is too big, we drop to POST (note that means that we can't use the HTTP cache any longer)
			// we are fine with that, requests to load > 1024 items are going to be rare


			if (isGet)// can use GET
			{
				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
				for (const auto& id : uniqueIds)
				{
					pathBuilder << "&id=" << ravendb::impl::Utils::url_escape(curl, id);
				}
			}
			else // ids too big, must use POST
			{
				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);

				auto json_str = nlohmann::json({ {"Ids", uniqueIds} }).dump();

				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());
			}

		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			long statusCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
			if (statusCode == 200)
				_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return true;
		}
	};
}
