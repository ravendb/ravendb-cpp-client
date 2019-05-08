#include "stdafx.h"
#include "http_extensions.h"
#include "CompareStringsIgnoreCase.h"

namespace ravendb::client::extensions::http_extensions
{
	std::optional<bool> get_boolean_header(const impl::CurlResponse& response, const std::string& header)
	{
		if(auto it = response.headers.find(header);
			it != response.headers.end())
		{
			return impl::utils::CompareStringsIgnoreCase::to_lower_str(it->second) == "true";
		}
		return {};
	}
}
