#include "pch.h"
#include "raven_test_definitions.h"

namespace ravendb::client::tests::infrastructure
{
	void set_for_fiddler(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	void set_no_proxy(CURL* curl)
	{

		curl_easy_setopt(curl, CURLOPT_NOPROXY, "*");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}
}