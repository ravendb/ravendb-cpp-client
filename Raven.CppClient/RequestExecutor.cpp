
#include "stdafx.h"
#include "RequestExecutor.h"
#include "GetDatabaseTopologyCommand.h"

using  
	ravendb::client::serverwide::commands::GetDatabaseTopologyCommand;

namespace ravendb::client::http
{
	RequestExecutor::~RequestExecutor() = default;

	void RequestExecutor::first_topology_update()
	{
		GetDatabaseTopologyCommand getTopology{};

		std::ostringstream errors;

		for (const auto& url : _initial_urls)
		{
			Topology result;
			try
			{
				auto server_node = ServerNode(url, _db_name, "?");
				result = execute_internal<Topology>(server_node, getTopology);
			}
			catch (RavenError& re)
			{
				errors << re.what() << '\n';
				if (re.get_error_type() == RavenError::ErrorType::DATABASE_DOES_NOT_EXIST)
				{
					throw RavenError(errors.str(), RavenError::ErrorType::DATABASE_DOES_NOT_EXIST);
				}
				continue;
			}
			// success
			std::atomic_store(&_topology, std::make_shared<Topology>(result));
			return;
		}

		throw RavenError(errors.str(), RavenError::ErrorType::GENERIC);
	}

	RequestExecutor::RequestExecutor(
		std::string db_name,
		std::vector<std::string> initial_urls,
		std::optional<impl::CertificateDetails> certificate_details,
		ravendb::client::impl::CurlOptionsSetter set_before_perform,
		ravendb::client::impl::CurlOptionsSetter set_after_perform)
		: _db_name(std::move(db_name))
		, _initial_urls(std::move(initial_urls))
		, _topology(std::make_shared<Topology>())
		, _certificate_details(std::move(certificate_details))
		, _curl_holder()
		, _set_before_perform(set_before_perform)
		, _set_after_perform(set_after_perform)
	{
		_topology->nodes.reserve(_initial_urls.size());

		for (const auto& url : _initial_urls)
		{
			_topology->nodes.emplace_back(url, _db_name, "?");
		}
		_topology->etag = -1;
	}

	void RequestExecutor::validate_urls()
	{
		const bool certificateHasHttps = _certificate_details.has_value();

		if (_initial_urls.empty())
			throw RavenError("No urls has been defined", RavenError::ErrorType::BAD_URL);

		for (auto& url : _initial_urls)
		{
			if (url.empty())
			{
				throw RavenError("Empty url is not supported", RavenError::ErrorType::BAD_URL);
			}
			if (url.back() == '/')
			{
				url.pop_back();
			}

			if (certificateHasHttps)
			{
				if (url.compare(0, 5, "https") != 0) // not a match
				{
					throw RavenError("Unable to use url '" + url + "', a certificate was specified, so url must use 'https' as the url scheme.",
						RavenError::ErrorType::BAD_URL);
				}
			}
			else if (url.compare(0, 5, "http:") != 0) // not a match
			{
				throw RavenError("Unable to use url '" + url + "', a certificate was NOT specified, so url must use 'http' as the url scheme.",
					RavenError::ErrorType::BAD_URL);
			}
		}
	}

	std::unique_ptr<RequestExecutor> RequestExecutor::create(
		std::vector<std::string> urls,
		std::string db,
		std::optional<impl::CertificateDetails> certificate_details,
		ravendb::client::impl::CurlOptionsSetter set_before_perform,
		ravendb::client::impl::CurlOptionsSetter set_after_perform)
	{
		//using explicit call to PRIVATE ctor
		auto rq = std::unique_ptr<RequestExecutor>(new RequestExecutor(
			std::move(db),
			std::move(urls),
			std::move(certificate_details),
			set_before_perform,
			set_after_perform));

		rq->validate_urls();

		if (! rq->_db_name.empty())
		{
			rq->first_topology_update();
		}
		return rq;
	}
}