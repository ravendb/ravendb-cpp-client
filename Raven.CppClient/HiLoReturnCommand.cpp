#include "stdafx.h"
#include "HiLoReturnCommand.h"

namespace ravendb::client::documents::commands
{
	HiLoReturnCommand::HiLoReturnCommand(std::string tag, int64_t last, int64_t end)
		: _tag(std::move(tag))
		, _last([&]()
	{
		if(last < 0)
		{
			throw std::invalid_argument("last is < 0");
		}
		return last;
	}())
		, _end([&]()
	{
		if (end < 0)
		{
			throw std::invalid_argument("end is < 0");
		}
		return end;
	}())
	{}

	void HiLoReturnCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream url_builder;

		url_builder << node->url << "/databases/" << node->database
			<< "/hilo/return?tag=" << _tag << "&end=" << _end << "&last=" << _last;

		std::string dummy{};

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_PUT, 1L);
		curl_ref.method = constants::methods::PUT;
		curl_easy_setopt(curl, CURLOPT_READDATA, &dummy);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)dummy.length());

		url_ref.emplace(url_builder.str());
	}
}
