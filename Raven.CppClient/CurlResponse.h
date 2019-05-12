#pragma once
#include "CurlHandlesHolder.h"

namespace ravendb::client::impl
{
	struct CurlResponse
	{
		std::string_view error{};
		std::string output{};
		std::unordered_map<std::string, std::string> headers{}; 
		long status_code{};
		CURLcode perform_result{};
	
	private:
		char error_buffer[CURL_ERROR_SIZE] = { '\0' };

	private:
		explicit CurlResponse(const CurlHandlesHolder::CurlReference& curl_ref);

	public:
		static CurlResponse run_curl_perform(const CurlHandlesHolder::CurlReference& curl_ref);

		CurlResponse();
	};
}
