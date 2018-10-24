
#include "stdafx.h"




raven::impl::CurlHolder::ReturnCurl raven::impl::CurlHolder::checkout_curl() 
{
	std::lock_guard<std::mutex> lk(_curlHandlesLock);
	if (_curlHandles.empty())
	{
		auto curl = _createCurlHandleMethod(_state);
		auto r = ReturnCurl(std::move(std::unique_ptr<CURL, CurlCleanup>(curl)), this);
		return r;
	}

	auto curl = std::move(_curlHandles.back());
	_curlHandles.pop_back();
	return CurlHolder::ReturnCurl(std::move(curl), this);
}

raven::impl::CurlHolder::CurlHolder(raven::impl::CreateCurlHandleMethod_t create_curl_instance, void* state) :
	_createCurlHandleMethod(create_curl_instance), _state(state) {
}


