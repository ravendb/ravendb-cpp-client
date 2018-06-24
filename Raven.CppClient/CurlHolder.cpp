#pragma once

#include "stdafx.h"


void raven::impl::CurlHolder::CurlCleanup::operator () (CURL* curl) {
	curl_easy_cleanup(curl);
}

void raven::impl::CurlHolder::return_curl(std::unique_ptr<CURL, raven::impl::CurlHolder::CurlCleanup> curl) {
	std::lock_guard<std::mutex> lk(_curl_guard);
	_curl_instances.push_back(std::move(curl));
}

raven::impl::CurlHolder::ReturnCurl raven::impl::CurlHolder::checkout_curl() {
	std::lock_guard<std::mutex> lk(_curl_guard);
	if (_curl_instances.empty()) {
		auto curl = _create_curl_instance(_state);
		auto r = ReturnCurl(std::move(std::unique_ptr<CURL, CurlCleanup>(curl)), this);
		return r;
	}
	else {
		auto curl = std::move(_curl_instances.back());
		_curl_instances.pop_back();
		return CurlHolder::ReturnCurl(std::move(curl), this);
	}
}

raven::impl::CurlHolder::CurlHolder(raven::impl::create_curl_instance create_curl_instance, void* state) :
	_create_curl_instance(create_curl_instance), _state(state) {
}


