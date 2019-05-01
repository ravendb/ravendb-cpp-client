#include "stdafx.h"
#include "CurlResponse.h"
#include "utils.h"
#include "CurlSListHolder.h"

namespace ravendb::client::impl
{
	const CurlResponse CurlResponse::run_curl_perform(const CurlHandlesHolder::CurlReference& curl_ref)
	{
		return CurlResponse(curl_ref);
	}

	CurlResponse::CurlResponse(const CurlHandlesHolder::CurlReference& curl_ref)
	{
		CURL* curl = curl_ref.get();
		CurlSListHolder headers_list{};

		for(const auto&[name, value] : curl_ref.headers)
		{
			std::ostringstream header{};
			header << name << ":" << value;
			headers_list.append(header.str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, curl_ref.url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list.get());
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::push_to_buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_buffer);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, utils::push_headers);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);

		if(curl_ref.set_before_perform && curl_ref.set_before_perform.value())
		{
			(*curl_ref.set_before_perform)(curl);
		}

		perform_result = curl_easy_perform(curl);

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

		if(curl_ref.set_after_perform && curl_ref.set_after_perform.value())
		{
			(*curl_ref.set_after_perform)(curl);
		}
	}
}
