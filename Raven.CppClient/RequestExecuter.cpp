#pragma once

#include "stdafx.h"


static CURL* default_create_curl_instance(void* state) {
	return curl_easy_init();
}

std::optional<raven::RavenError> raven::RequestExecutor::first_topology_update() {

	auto getTopology = GetDatabaseTopologyCommand();
	std::string errors;

	for (auto url : _initialUrls) {
		auto serverNode = ServerNode(url, _dbName, "?");
		auto result = execute<GetDatabaseTopologyCommand, Topology>(serverNode, getTopology);
		if (result.error.has_value())
		{
			errors.append(result.error.value().text).append("\n");

			if (result.error.value().type == RavenError::database_does_not_exists)
				return { { errors, RavenError::database_does_not_exists } };

			continue;
		}

		auto topology = std::move(result.value);

		std::atomic_store(&_topology, std::make_shared<Topology>(topology));

		return {}; // success
	}

	return { { errors, RavenError::generic } };
}


raven::RequestExecutor::RequestExecutor(std::vector<std::string> initialUrls, std::string db, std::string certificate, std::unique_ptr<raven::impl::CurlHolder> curl_holder)
	: _initialUrls(initialUrls), _certificate(certificate), _dbName(db), _curl_holder(std::move(curl_holder))
{
	_topology = std::make_shared<Topology>();
	for (auto url : initialUrls) {
		_topology->nodes.push_back(ServerNode(url, db, "?"));
	}
	_topology->etag = -1;
}

std::optional<raven::RavenError> raven::RequestExecutor::validate_urls() {
	bool certificateHasHttps = _certificate.empty() == false;
	for (auto & url : _initialUrls) {
		if (url.empty()) {
			return { { "Empty url is not supported", RavenError::bad_url } };
		}
		if (url[url.size() - 1] == '/') {
			return { { "Url '" + url + "' must not end with '/'", RavenError::bad_url } };
		}

		if (certificateHasHttps) {
			if (url.compare(0, 5, "https") != 0) { // not a match
				return { { "Unable to use url '" + url + "', a certificate was specified, so url must use 'https' as the url scheme.", RavenError::bad_url } };
			}
		}
		else if (url.compare(0, 5, "http:") != 0) { // not a match
			return { { "Unable to use url '" + url + "', a certificate was NOT specified, so url must use 'http' as the url scheme.", RavenError::bad_url } };
		}
	}

	if (_initialUrls.size() == 0)
		return { { "No urls has been defined", RavenError::bad_url } };

	return {};
}


raven::Result<std::unique_ptr<raven::RequestExecutor>> raven::RequestExecutor::create(
	std::vector<std::string> urls, 
	std::string db,
	std::string certificate,
	std::pair<raven::impl::create_curl_instance, void*> create_curl) {
	if (create_curl.first == NULL)
		create_curl.first = default_create_curl_instance;

	auto holder = std::make_unique<raven::impl::CurlHolder>(create_curl.first, create_curl.second);

	auto rq = std::make_unique<RequestExecutor>(urls, db, certificate, std::move(holder));

	auto status = rq->validate_urls();
	if (status.has_value())
		return status.value();

	auto result = rq->first_topology_update();
	if (result.has_value())
		return result.value();

	return Result(std::move(rq), RavenError::success);
}
