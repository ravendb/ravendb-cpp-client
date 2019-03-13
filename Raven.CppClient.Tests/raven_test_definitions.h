#pragma once
#include <curl/curl.h>

namespace ravendb::client::tests::infrastructure
{
	constexpr char UNSECURED_RE_DETAILS_FILE_NAME[] = "../unsecured_re.txt";
	constexpr char SECURED_RE_DETAILS_FILE_NAME[] = "../secured_re.txt";

#define TEST_NAME (ravendb::client::documents::conventions::DocumentConventions::default_get_collection_name(typeid(decltype(*this))))

	//using fiddler + verbose
	void set_for_fiddler(CURL* curl);

	//no_proxy + verbose
	void set_no_proxy(CURL* curl);

}
