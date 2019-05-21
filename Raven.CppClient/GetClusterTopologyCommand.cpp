#include "stdafx.h"
#include "GetClusterTopologyCommand.h"

namespace ravendb::client::serverwide::commands
{
	GetClusterTopologyCommand::~GetClusterTopologyCommand() = default;

	GetClusterTopologyCommand::GetClusterTopologyCommand(std::optional<std::string> debug_tag)
		: _debug_tag(std::move(debug_tag))
	{}

	void GetClusterTopologyCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream url_builder;
		url_builder << node->url << "/cluster/topology";

		if (_debug_tag)
		{
			url_builder << "?" << *_debug_tag;
		}

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(url_builder.str());
	}

	void GetClusterTopologyCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}
			_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool GetClusterTopologyCommand::is_read_request() const
	{
		return true;
	}
}
