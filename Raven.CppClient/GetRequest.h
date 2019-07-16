#pragma once
#include <memory>
#include "CompareStringsLessThanIgnoreCase.h"

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
		std::map<std::string, std::string, impl::utils::CompareStringsLessThanIgnoreCase> headers{};
		std::optional<std::string> query{};
		std::string method{};
		std::unique_ptr<IContent> content{};

		std::string get_url_and_query() const;
	};

	inline GetRequest::IContent::~IContent() = default;
}
