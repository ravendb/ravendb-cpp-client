#pragma once
#include "RavenException.h"
#include "CurlResponse.h"

namespace ravendb::client::exceptions
{
	class CurlException : public RavenException
	{
	public:
		const CURLcode _perform_result;

	public:
		~CurlException() = default;

		explicit CurlException(const impl::CurlResponse& response)
			: RavenException(response.error.data())
			, _perform_result(response.perform_result)
		{}
	};
}
