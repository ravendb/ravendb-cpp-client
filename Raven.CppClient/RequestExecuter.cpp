
#include "stdafx.h"
#include "GetDatabaseTopologyCommand.h"
#include "RequestExecutor.h"




void raven::RequestExecutor::first_topology_update()
{
	GetDatabaseTopologyCommand getTopology;

	std::ostringstream errors;

	for (const auto& url : _initialUrls)
	{
		Topology result;
		try
		{
			auto serverNode = ServerNode(url, _dbName, "?");
			result = std::move(execute<GetDatabaseTopologyCommand, Topology>(serverNode, getTopology));
		}
		catch (RavenError& re)
		{
			errors << re.what() << '\n';
			if(re.get_error_type() == RavenError::ErrorType::database_does_not_exists)
			{
				throw RavenError(errors.str(), RavenError::ErrorType::database_does_not_exists);
			}
			continue;
		}

		//if (result.error.has_value())
		//{
		//	errors.append(result.error.value().text).append("\n");

		//	if (result.error.value().type == RavenError::database_does_not_exists)
		//		return { { errors, RavenError::database_does_not_exists } };

		//	continue;
		//}

		//auto topology = std::move(result.value);

		// success
		std::atomic_store(&_topology, std::make_shared<Topology>(result));
		return;
	}

	throw RavenError(errors.str(), RavenError::ErrorType::generic);
}


raven::RequestExecutor::RequestExecutor (
	std::string db_name,
	std::vector<std::string> initialUrls,
	std::string certificate,
	std::unique_ptr<raven::impl::CurlHandlesHolder> curl_holder)
	: _db_name(std::move(db_name))
	, _initial_urls(std::move(initialUrls))
	, _topology(std::make_shared<Topology>())
	, _certificate(std::move(certificate))
	, _curl_holder(std::move(curl_holder))
{
	_topology->nodes.reserve(_initial_urls.size());

	for (const auto& url : _initial_urls) 
	{
		_topology->nodes.emplace_back(url, _db_name, "?");
	}
	_topology->etag = -1;
}

void raven::RequestExecutor::validate_urls()
{
	const bool certificateHasHttps = not _certificate.empty();

	if (_initialUrls.empty())
		throw RavenError("No urls has been defined", RavenError::ErrorType::bad_url);

	for (auto & url : _initialUrls) 
	{
		if (url.empty()) 
		{
			throw RavenError("Empty url is not supported", RavenError::ErrorType::bad_url);
		}
		if (url.back() == '/')
		{
			url.pop_back();
		}
		//{
		//	return { {[&](auto url_str) -> std::string {
		//			std::ostringstream oss;
		//			oss << "Url '" << url_str << "' must not end with '/'";
		//			return oss.str();
		//			}(url),RavenError::bad_url } };
		//	//{ "Url '" + url + "' must not end with '/'", RavenError::bad_url } };
		//}

		if (certificateHasHttps)
		{
			if (url.compare(0, 5, "https") != 0) // not a match
			{
				throw RavenError("Unable to use url '" + url + "', a certificate was specified, so url must use 'https' as the url scheme.",
					RavenError::ErrorType::bad_url);
			}
		}else if (url.compare(0, 5, "http:") != 0) // not a match
		{
			throw RavenError("Unable to use url '" + url + "', a certificate was NOT specified, so url must use 'http' as the url scheme.",
				RavenError::ErrorType::bad_url);
		}
	}
}


std::unique_ptr<raven::RequestExecutor> raven::RequestExecutor::create(
	std::vector<std::string> urls,
	std::string db,
	std::string certificate,
	std::pair<raven::impl::CreateCurlHandleMethod_t, void*> create_curl)
{
	auto holder = std::make_unique<raven::impl::CurlHandlesHolder>(create_curl.first, create_curl.second);

	auto rq = std::unique_ptr<RequestExecutor>(new RequestExecutor(std::move(urls), std::move(db), std::move(certificate), std::move(holder)));

	auto status = rq->validate_urls();
	if (status.has_value())
		return status.value();

	auto result = rq->first_topology_update();
	if (result.has_value())
		return result.value();

	return Result(std::move(rq), RavenError::success);
}
