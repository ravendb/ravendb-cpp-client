#include "stdafx.h"
#include "WaitForRaftIndexCommand.h"
#include "ServerNode.h"
#include "Constants.h"

namespace ravendb::client::serverwide::commands
{
	WaitForRaftIndexCommand::~WaitForRaftIndexCommand() = default;

	WaitForRaftIndexCommand::WaitForRaftIndexCommand(int64_t rachis_index)
		: _rachis_index(rachis_index)
	{}

	void WaitForRaftIndexCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream url_builder;

		url_builder << node->url << "/rachis/waitfor?index=" << _rachis_index;

		_requested_node = node;

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;
		url_ref.emplace(url_builder.str());
	}

	std::shared_ptr<const http::ServerNode> WaitForRaftIndexCommand::get_requested_node() const
	{
		return _requested_node;
	}

	void WaitForRaftIndexCommand::set_requested_node(std::shared_ptr<const http::ServerNode> requested_node)
	{
		_requested_node = requested_node;
	}

	void WaitForRaftIndexCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if(!response)
		{
			throw_invalid_response();
		}
	}

	bool WaitForRaftIndexCommand::is_read_request() const
	{
		return true;
	}
}