#include "stdafx.h"
#include "utils.h"

static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

static inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}


size_t ravendb::client::impl::Utils::push_headers(char *buffer, size_t size, size_t nitems, void *userdata)
{
	auto real_size = size * nitems;
	auto headers = static_cast<std::map<std::string, std::string>*>(userdata);
	auto result = static_cast<char*>(memchr(buffer, ':', real_size));
	if (result == NULL) // doesn't have ':', probably not a header line, not interesting
		return real_size;

	auto header_name_size = result - buffer;

	auto header_name = std::string(buffer, header_name_size);

	auto header_val = std::string(result + 1, real_size - header_name_size - 1);
	// remove starting space and \r\n at end

	ltrim(header_val);
	rtrim(header_val);

	headers->emplace(header_name, header_val);

	return real_size;
}

size_t ravendb::client::impl::Utils::push_to_buffer(char* contents, size_t size, size_t nmemb, void* output_buffer_void)
{
	auto real_size = size * nmemb;
	auto& output_buffer = *static_cast<std::string*>(output_buffer_void);
	output_buffer.append((char*)contents, real_size);
	return real_size;
}

std::string ravendb::client::impl::Utils::url_escape(CURL* curl, const std::string& val)
{
	auto result = curl_easy_escape(curl, val.c_str(), (int)val.length());
	if (result == nullptr)
	{
		throw std::invalid_argument("url escape failure on: " + val);
	}
	auto str = std::string(result);
	curl_free(result);
	return str;
}

size_t ravendb::client::impl::Utils::read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	auto str = static_cast<const std::string*>(stream);
	size_t length = str->length();

	std::memcpy(ptr, str->c_str(), length);
	return length;
}
