#pragma once
#include "CurlHandlesHolder.h"

namespace ravendb::client::impl
{
	struct CurlResponse
	{
		char error_buffer[CURL_ERROR_SIZE] = { '\0' };
		std::string output_buffer{};
		std::unordered_map<std::string, std::string> headers{}; 
		long status_code{};
		CURLcode perform_result{};

	private:
		explicit CurlResponse(const CurlHandlesHolder::CurlReference& curl_ref);

	public:
		static const CurlResponse run_curl_perform(const CurlHandlesHolder::CurlReference& curl_ref);
	};
}
