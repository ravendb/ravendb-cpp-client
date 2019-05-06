#include "stdafx.h"
#include "RavenCommand.h"

namespace ravendb::client::http
{
	std::string url_encode(CURL* curl, const std::string& value)
	{
		char* result = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));

		if (result == nullptr)
		{
			throw std::invalid_argument("url escape failed on: " + value);
		}

		auto str = std::string(result);
		curl_free(result);

		return str;
	}

	std::string url_encode(const impl::CurlHandlesHolder::CurlReference& curl_ref, const std::string& value)
	{
		return url_encode(curl_ref.get(), value);
	}

	std::string url_encode(const std::string& value)
	{
		auto curl = curl_easy_init();
		auto result = url_encode(curl, value);
		curl_easy_cleanup(curl);

		return result;
	}
}
