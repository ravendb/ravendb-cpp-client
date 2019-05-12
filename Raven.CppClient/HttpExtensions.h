#pragma once
#include "CurlResponse.h"

namespace ravendb::client::extensions
{
	class HttpExtensions
	{
	public:
		HttpExtensions() = delete;
		~HttpExtensions() = delete;

	private:
		static std::string etag_header_to_change_vector(const std::string& response_header);

	public:
		static std::string get_required_etag_header(const impl::CurlResponse& response);

		static std::optional<bool> get_boolean_header(const impl::CurlResponse& response, const std::string& header);

	};
}
