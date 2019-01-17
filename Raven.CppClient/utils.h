#pragma once
#include "stdafx.h"

namespace ravendb::client::impl::utils
{
	constexpr char DEFAULT_CA_BUNDLE_PATH[] = "../ca-bundle.crt";

	size_t push_headers(char *buffer, size_t size, size_t nitems, void *userdata);

	size_t push_to_buffer(char* contents, size_t size, size_t nmemb, void* output_buffer_void);

	//expects const std::string* in stream
	size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream);

	//expects SSL_CTX* in sslctx_void and const CertificateDetails* in cert_details_void
	CURLcode sslctx_function(CURL*, void *sslctx_void, void *cert_details_void);

	std::string url_escape(CURL* curl, const std::string& val);

	bool is_blank(const std::string& str);//TODO use it for validate string arguments

	// serialization in C# TimeSpan format : d.hh:mm:ss.sssssss or hh:mm:ss.sssssss
	std::string millis_to_timespan(const std::chrono::milliseconds& msec);

	struct CompareStringsIgnoreCase
	{
		static std::string to_lower_str(const std::string& str)
		{
			std::string temp{};
			std::transform(str.cbegin(), str.cend(), std::back_insert_iterator<std::string>(temp),
				[](std::string::value_type c) {return std::tolower(c); });
			return temp;
		};

		bool operator()(const std::string& s1, const std::string s2) const
		{
			return to_lower_str(s1) < to_lower_str(s2);
		}
	};
}


