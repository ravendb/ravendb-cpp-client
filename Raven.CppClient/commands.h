#pragma once

#include "ravendb.h"
#include "utils.h"


namespace raven
{

	template<typename TResult>
	class RavenCommand
	{
	
	protected:
		TResult _result;
		
	public:
		virtual ~RavenCommand() = 0;

		virtual void createRequest(const ServerNode& node, std::string& url, CURL* curl) const = 0;

		virtual void setResponse(CURL* curl, const nlohmann::json& response) = 0;

		virtual void setResponseNotFound(CURL* curl)
		{
			_result = TResult();
		}

		virtual TResult getResult() const
		{
			return _result;
		}
	};

	template <typename TResult>
	RavenCommand<TResult>::~RavenCommand() = default;


	class GetDatabaseTopologyCommand :public RavenCommand<Topology>
	{
	public:

		~GetDatabaseTopologyCommand() override = default;

		void createRequest(const ServerNode& node, std::string& url, CURL* curl) const override
		{
			std::ostringstream urlBuilder;

			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			urlBuilder << node.url << "/topology?name=" << node.database;

			url = urlBuilder.str();
		}
		void setResponse(CURL* curl, const nlohmann::json& response) override
		{
			long statusCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
			if (statusCode == 200)
				_result = response;
		}
	};

	class GetDocumentsCommand : public RavenCommand<GetDocumentsResult>
	{
	private:	
		std::string _id;

		std::set<std::string> _ids;
		std::set<std::string> _includes;

		bool _metadataOnly = false;

		std::string _startWith;
		std::string _startAfter;
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
			, _includes(includes.cbegin(),includes.cend())
			, _metadataOnly(_metadataOnly)
		{}

		GetDocumentsCommand(const std::vector<std::string>& ids, const std::vector<std::string>& includes, bool _metadataOnly)
			: _ids([&] 
				{
					if (ids.empty()) 
						throw std::invalid_argument("Please supply at least one id");
					else return std::move(std::set<std::string>(ids.cbegin(),ids.cend()));
				}())
			, _includes(includes.cbegin(), includes.cend())
			, _metadataOnly(_metadataOnly)
		{
			
		}

		GetDocumentsCommand
			( std::string startWith
			, std::string startAfter
			, std::string matches
			, std::string exclude
			, int32_t start
			, int32_t pageSize
			, bool metadataOnly)
			: _metadataOnly(metadataOnly)
			, _startWith(std::move(startWith))
			, _startAfter(std::move(startAfter))
			, _matches(std::move(matches))
			, _exclude(std::move(exclude))
			, _start(start)
			, _pageSize(pageSize)
			
		{}

		void createRequest(const ServerNode& node, std::string& url, CURL* curl) const override
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

			if (not _startWith.empty()) 
			{
				pathBuilder << "&startsWith=" << raven::impl::Utils::url_escape(curl, _startWith);

				if (not _matches.empty())
				{
					pathBuilder << "&matches=" << raven::impl::Utils::url_escape(curl, _matches);
				}

				if (not _exclude.empty())
				{
					pathBuilder << "&exclude=" << raven::impl::Utils::url_escape(curl, _exclude);
				}

				if (not _startAfter.empty())
				{
					pathBuilder << "&startAfter=" << raven::impl::Utils::url_escape(curl, _startAfter);
				}
			}

			for (auto const& include : _includes)
			{
				pathBuilder << "&include=" << include;
			}

			if (not _id.empty())
			{
				pathBuilder << "&id=" << raven::impl::Utils::url_escape(curl, _id);
			}
			else if (not _ids.empty())
			{
				prepareRequestWithMultipleIds(pathBuilder, curl);
			}

			url = pathBuilder.str();
		}

	private:
		void prepareRequestWithMultipleIds(std::ostringstream& pathBuilder, CURL* curl) const 
		{
			std::size_t totalLen = 0;
			std::for_each(_ids.cbegin(), _ids.cend(), [&](std::string str) {totalLen += str.length(); });

			bool isGet = totalLen < 1024;
			// if it is too big, we drop to POST (note that means that we can't use the HTTP cache any longer)
			// we are fine with that, requests to load > 1024 items are going to be rare
			

			if (isGet)// can use GET
			{
				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
				for (const auto& id : _ids)
				{
					pathBuilder << "&id=" << raven::impl::Utils::url_escape(curl, id);
				}
			}
			else // ids too big, must use POST
			{
				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);

				auto json_str = nlohmann::json({{"Ids", _ids}}).dump();

				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());
			}

		}
	public:

		void setResponse(CURL* curl, const nlohmann::json& response) override
		{
			_result = response;
		}
	};
}
