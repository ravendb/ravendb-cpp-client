#pragma once
#include <curl/curl.h>
#include "GetCppClassName.h"

namespace ravendb::client::tests::infrastructure
{
#ifdef _WIN32
	constexpr char UNSECURED_RE_DETAILS_FILE_NAME[] = "unsecured_re.txt";
	constexpr char SECURED_RE_DETAILS_FILE_NAME[] = "secured_re.txt";
#endif
#ifdef  __unix__
    constexpr char UNSECURED_RE_DETAILS_FILE_NAME[] = "unsecured_re.txt";
	constexpr char SECURED_RE_DETAILS_FILE_NAME[] = "secured_re.txt";
#endif

#define TEST_NAME (ravendb::client::impl::utils::GetCppClassName::get_simple_class_name(typeid(decltype(*this))))

	//using fiddler + verbose
	void set_for_fiddler(CURL* curl);

	//no_proxy + verbose
	void set_no_proxy(CURL* curl);

}
