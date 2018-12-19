#pragma once
#include "stdafx.h"

namespace ravendb::client::impl::utils
{
	size_t push_headers(char *buffer, size_t size, size_t nitems, void *userdata);

	size_t push_to_buffer(char* contents, size_t size, size_t nmemb, void* output_buffer_void);

	//expects const std::string* in stream
	size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream);

	//expects SSL_CTX* in sslctx_void and const CertificateDetails* in cert_details_void
	CURLcode sslctx_function(CURL*, void *sslctx_void, void *cert_details_void);

	std::string url_escape(CURL* curl, const std::string& val);
}


