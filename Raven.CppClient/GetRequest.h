#pragma once

namespace ravendb::client::impl::utils
{
	struct CompareStringsIgnoreCase;
}

namespace ravendb::client::documents::commands::multi_get
{
	struct GetRequest
	{
		struct IContent
		{
			virtual ~IContent() = 0;

			virtual void write_content(nlohmann::json& json) const = 0;
		};

		std::string url{};
		std::map<std::string, std::string, impl::utils::CompareStringsIgnoreCase> headers{};
		std::optional<std::string> query{};
		std::string method{};
		std::unique_ptr<IContent> content{};

		std::string get_url_and_query() const
		{
			if(!query)
			{
				return url;
			}

			return url + ((query->size() > 0 && query->front() == '?') ? "" : "?") + *query;
		}
	};

	inline GetRequest::IContent::~IContent() = default;
}
