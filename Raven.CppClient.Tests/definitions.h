#pragma once
#include <curl/curl.h>

const std::string RAVEN_SERVER_URL = "http://localhost:8080";

//using fiddler + verbose
inline void set_for_fiddler(CURL* curl)
{
	curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
}
