#include "pch.h"
#include "definitions.h"

using namespace ravendb::client;

//using fiddler + verbose
void set_for_fiddler(CURL* curl)
{
	curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
}

//request executor only - no DB is created
std::unique_ptr<http::RequestExecutor> get_raw_request_executor(const std::string& db)
{
	return http::RequestExecutor::create({ RAVEN_SERVER_URL }, db, {}, set_for_fiddler);
}

