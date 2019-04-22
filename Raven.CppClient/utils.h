#pragma once
#include <curl/curl.h>

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

	//Useful for validating string arguments
	bool is_blank(const std::string& str);

	template<typename T>
	std::shared_ptr<T> default_value()
	{
		if constexpr (std::is_fundamental_v<T>)
		{
			return std::make_shared<T>();
		}
		else return std::shared_ptr<void>();
	}
}

bool operator==(const std::tm& lhs, const std::tm& rhs);

bool operator!=(const std::tm& lhs, const std::tm& rhs);


