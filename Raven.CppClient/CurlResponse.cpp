#include "stdafx.h"
#include "CurlResponse.h"
#include "CurlSListHolder.h"
#include "utils.h"

namespace
{
	size_t push_headers(char *buffer, size_t size, size_t nitems, void *userdata)
	{
		const auto real_size = size * nitems;
		auto& headers = *static_cast<std::unordered_map<std::string, std::string>*>(userdata);
		auto delimiter = static_cast<char*>(memchr(buffer, ':', real_size));
		if (delimiter == nullptr) // doesn't have ':', probably not a header line, not interesting
			return real_size;

		auto header_name_size = delimiter - buffer;

		auto header_name = std::string(buffer, header_name_size);

		auto header_val = std::string(delimiter + 1, real_size - header_name_size - 1);
		// remove starting space and \r\n at end
		ravendb::client::impl::utils::left_trim(header_val);
		ravendb::client::impl::utils::right_trim(header_val);

		headers.emplace(header_name, header_val);

		return real_size;
	}

	size_t push_to_stream(char* contents, size_t size, size_t nmemb, void* output_stream_ptr)
	{
		const auto real_size = size * nmemb;
		auto& output_stream = *static_cast<std::ostringstream*>(output_stream_ptr);
		auto prev_pros = output_stream.tellp();

		output_stream.write(contents, real_size);

		auto curr_pos = output_stream.tellp();
		if(curr_pos == std::ostringstream::pos_type(-1))
		{
			return 0;
		}

		return size_t(curr_pos - prev_pros);
	}
}

namespace ravendb::client::impl
{
	CurlResponse CurlResponse::run_curl_perform(const CurlHandlesHolder::CurlReference& curl_ref)
	{
		return CurlResponse(curl_ref);
	}

	CurlResponse::CurlResponse(const CurlResponse& other)\
		: _valid(other._valid)
		, output(other.output.str())
		, headers(other.headers)
		, status_code(other.status_code)
		, perform_result(other.perform_result)
	{
#ifdef _MSC_VER
		strcpy_s(_error_buffer, other._error_buffer);
#else
		std::strcpy(_error_buffer, other._error_buffer);
#endif
		error = _error_buffer;
	}

	CurlResponse::CurlResponse(CurlResponse&& other) noexcept
		: _valid(other._valid)
		, output(std::move(other.output))
		, headers(std::move(other.headers))
		, status_code(other.status_code)
		, perform_result(other.perform_result)
	{
#ifdef _MSC_VER
		strcpy_s(_error_buffer, other._error_buffer);
#else
		std::strcpy(_error_buffer, other._error_buffer);
#endif
		error = _error_buffer;
	}

	CurlResponse& CurlResponse::operator=(const CurlResponse& other)
	{
		if (this == &other)
			return *this;

		_valid = other._valid;
#ifdef _MSC_VER
		strcpy_s(_error_buffer, other._error_buffer);
#else
		std::strcpy(_error_buffer, other._error_buffer);
#endif
		error = _error_buffer;
		output << other.output.rdbuf();
		headers = other.headers;
		status_code = other.status_code;

		return *this;
	}

	CurlResponse& CurlResponse::operator=(CurlResponse&& other) noexcept
	{
		if (this == &other)
			return *this;

		_valid = other._valid;
#ifdef _MSC_VER
		strcpy_s(_error_buffer, other._error_buffer);
#else
		std::strcpy(_error_buffer, other._error_buffer);
#endif
		error = _error_buffer;
		output = std::move(other.output);
		headers = std::move(other.headers);
		status_code = other.status_code;

		return *this;
	}

	bool CurlResponse::is_valid() const
	{
		return _valid;
	}

	CurlResponse::CurlResponse(const CurlHandlesHolder::CurlReference& curl_ref)
	{
		CURL* curl = curl_ref.get();
		CurlSListHolder headers_list{};

		for(const auto&[name, value] : curl_ref.headers)
		{
			std::ostringstream header{};
			header << name << ": " << value;
			headers_list.append(header.str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, curl_ref.url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list.get());
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, _error_buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, push_to_stream);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, push_headers);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);

		if(curl_ref.set_before_perform && curl_ref.set_before_perform.value())
		{
			(*curl_ref.set_before_perform)(curl);
		}

		perform_result = curl_easy_perform(curl);

		error = _error_buffer;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, nullptr);

		if(curl_ref.set_after_perform && curl_ref.set_after_perform.value())
		{
			(*curl_ref.set_after_perform)(curl);
		}
	}
}
