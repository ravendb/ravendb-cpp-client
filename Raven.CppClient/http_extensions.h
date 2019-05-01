#pragma once
#include "CurlResponse.h"

namespace ravendb::client::extensions::http_extensions
{
	std::optional<bool> get_boolean_header(const impl::CurlResponse& response, const std::string& header);
}
