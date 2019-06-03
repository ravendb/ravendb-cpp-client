#include "stdafx.h"
#include "HttpExtensions.h"
#include "CompareStringsEqualIgnoreCase.h"
#include "Constants.h"

namespace ravendb::client::extensions
{
	std::string HttpExtensions::etag_header_to_change_vector(const std::string& response_header)
	{
		if(impl::utils::is_blank(response_header))
		{
			throw std::invalid_argument("'response_header' is blank");
		}
		if(response_header.front() == '"')
		{
			return response_header.substr(1, response_header.length() - 2);
		}
		return response_header;
	}

	std::string HttpExtensions::get_required_etag_header(const impl::CurlResponse& response)
	{
		if(auto it = response.headers.find(constants::headers::ETAG);
			it != response.headers.end())
		{
			if(!impl::utils::is_blank(it->second))
			{
				return etag_header_to_change_vector(it->second);
			}
			else
			{
				throw std::runtime_error("Response had an empty ETag header");
			}
		}
		else
		{
			throw std::runtime_error("Response didn't had an ETag header");
		}
	}

	std::optional<std::string> HttpExtensions::get_etag_header(const impl::CurlResponse& response)
	{
		if(auto it = response.headers.find(constants::headers::ETAG);
			it != response.headers.end())
		{
			return etag_header_to_change_vector(it->second);
		}

		return {};
	}

	std::optional<std::string> HttpExtensions::get_etag_header(const std::map<std::string, std::string,
		impl::utils::CompareStringsLessThanIgnoreCase>& headers)
	{
		if (auto it = headers.find(constants::headers::ETAG);
			it != headers.end())
		{
			return etag_header_to_change_vector(it->second);
		}

		return {};
	}

	std::optional<bool> HttpExtensions::get_boolean_header(const impl::CurlResponse& response,
		const std::string& header)
	{
		if (auto it = response.headers.find(header);
			it != response.headers.end())
		{
			return impl::utils::CompareStringsEqualIgnoreCase()(it->second, "true");
		}
		return {};
	}
}
