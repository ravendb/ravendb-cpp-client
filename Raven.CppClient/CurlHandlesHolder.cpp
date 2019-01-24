#include "stdafx.h"
#include "CurlHandlesHolder.h"

namespace ravendb::client::impl
{
	CurlHandlesHolder::CurlGlobalInit CurlHandlesHolder::_curl_global_init{};

	CurlHandlesHolder::CurlGlobalInit::CurlGlobalInit()
	{
		const auto res = curl_global_init(CURL_GLOBAL_ALL);
		if (res != CURLE_OK)
		{
			throw std::runtime_error("curl_global_init failed !");
		}
	}

	CurlHandlesHolder::CurlGlobalInit::~CurlGlobalInit()
	{
		curl_global_cleanup();
	}

	void CurlHandlesHolder::return_curl_handle_to_holder(CurlHandleUniquePtr curl)
	{
		std::lock_guard<std::mutex> lg(_curl_handles_lock);
		_curl_handles.push(std::move(curl));
	}

	CurlHandlesHolder::ReturnCurl::~ReturnCurl()
	{
		curl_easy_reset(this->get());
		_holder->return_curl_handle_to_holder(std::move(_curl));
	}

	CURL* CurlHandlesHolder::ReturnCurl::get() const
	{
		if (_curl)
		{
			return _curl.get();
		}
		else
		{
			throw std::runtime_error("ReturnCurl object does not contain valid curl handle");
		}
	}

	CurlHandlesHolder::ReturnCurl CurlHandlesHolder::checkout_curl()
	{
		std::lock_guard<std::mutex> lg(_curl_handles_lock);
		if (_curl_handles.empty())
		{
			auto curl = curl_easy_init();
			if (curl == nullptr)
			{
				throw std::runtime_error("curl handle creation failed");
			}
			return { CurlHandleUniquePtr(curl), *this };
		}

		auto curl = std::move(_curl_handles.front());
		_curl_handles.pop();
		return { std::move(curl), *this };
	}
}


