#pragma once
#include <curl/curl.h>

namespace ravendb::client::impl::utils
{
	constexpr char DEFAULT_CA_BUNDLE_PATH[] = "../ca-bundle.crt";

	//expects SSL_CTX* in sslctx_void and const CertificateDetails* in cert_details_void
	CURLcode sslctx_function(CURL*, void *sslctx_void, void *cert_details_void);

	//Useful for validating string arguments
	bool is_blank(const std::string& str);

	//transform string to lower case
	std::string to_lower_str(const std::string& str);

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


