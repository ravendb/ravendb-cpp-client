#include "stdafx.h"
#include "GetTcpInfoCommand.h"
#include "ServerNode.h"
#include "Constants.h"
#include "utils.h"

namespace ravendb::client::serverwide::commands
{
	GetTcpInfoCommand::~GetTcpInfoCommand() = default;

	GetTcpInfoCommand::GetTcpInfoCommand(std::string tag, std::optional<std::string> db_name)
		: _tag(std::move(tag))
		, _db_name(db_name && !impl::utils::is_blank(*db_name) ? std::move(db_name) : std::optional<std::string>())
	{}

	void GetTcpInfoCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream url_builder;

		if (_db_name)
		{
			url_builder << node->url << "/databases/" << *_db_name << "/info/tcp?tag=" << _tag;
		}
		else
		{
			url_builder << node->url << "/info/tcp?tcp=" << _tag;
		}

		_requested_node = node;

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(url_builder.str());
	}

	std::shared_ptr<const http::ServerNode> GetTcpInfoCommand::get_requested_node() const
	{
		return _requested_node;
	}

	void GetTcpInfoCommand::set_requested_node(std::shared_ptr<const http::ServerNode> requested_node)
	{
		_requested_node = requested_node;
	}

	void GetTcpInfoCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if(!response)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool GetTcpInfoCommand::is_read_request() const
	{
		return true;
	}
}
