#pragma once
#include "CurlHandlesHolder.h"

namespace ravendb::client::impl
{
	struct CurlResponse
	{
	private:
		bool _valid = true;

		char _error_buffer[CURL_ERROR_SIZE] = { '\0' };

	public:
		std::string_view error{};
		std::string output{};
		std::unordered_map<std::string, std::string> headers{}; 
		long status_code{};
		CURLcode perform_result{};

	private:
		explicit CurlResponse(const CurlHandlesHolder::CurlReference& curl_ref);
	public:
		static CurlResponse run_curl_perform(const CurlHandlesHolder::CurlReference& curl_ref);

		CurlResponse() : _valid(false){}

		bool is_valid() const;
	};
}
