#pragma once
#include <curl/curl.h>
#include <string>
#include <memory>
#include <ctime>

namespace ravendb::client::impl::utils
{
    //TODO think of a better solution
    //TODO do not forget to UPDATE the ca_bundle
#ifndef DEFAULT_CA_BUNDLE_PATH

	constexpr char DEFAULT_CA_BUNDLE_PATH_VALUE[] = "../ca_bundle/ca-bundle.crt";

#else

    constexpr char DEFAULT_CA_BUNDLE_PATH_VALUE[] = DEFAULT_CA_BUNDLE_PATH;

#endif

	//expects SSL_CTX* in sslctx_void and const CertificateDetails* in cert_details_void
	CURLcode sslctx_function(CURL*, void *sslctx_void, void *cert_details_void);

	//Useful for validating string arguments
	bool is_blank(const std::string& str);

	//transform string to lower case
	std::string to_lower_str(const std::string& str);

	void left_trim(std::string &s);

	void right_trim(std::string &s);

	template<typename T>
	std::shared_ptr<T> default_value()
	{
		if constexpr (std::is_fundamental_v<T>)
		{
			return std::make_shared<T>();
		}
		else return std::shared_ptr<T>();
	}
}

bool operator==(const std::tm& lhs, const std::tm& rhs);

bool operator!=(const std::tm& lhs, const std::tm& rhs);


