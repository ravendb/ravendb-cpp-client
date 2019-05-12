#include "stdafx.h"
#include "GetDatabaseNamesOperation.h"
#include "json_utils.h"

namespace ravendb::client::serverwide::operations
{
	GetDatabaseNamesOperation::~GetDatabaseNamesOperation() = default;

	GetDatabaseNamesOperation::GetDatabaseNamesOperation(int32_t _start, int32_t _page_size)
		: _start(_start)
		, _page_size(_page_size)
	{}

	std::unique_ptr<http::RavenCommand<std::vector<std::string>>> GetDatabaseNamesOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions)
	{
		return std::make_unique<GetDatabaseNamesCommand>(_start, _page_size);
	}

	GetDatabaseNamesOperation::GetDatabaseNamesCommand::~GetDatabaseNamesCommand() = default;

	GetDatabaseNamesOperation::GetDatabaseNamesCommand::GetDatabaseNamesCommand(int32_t _start, int32_t _page_size)
		: _start(_start)
		, _page_size(_page_size)
	{}

	void GetDatabaseNamesOperation::GetDatabaseNamesCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases?start=" << _start
			<< "&pageSize=" << _page_size << "&namesOnly=true";

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void GetDatabaseNamesOperation::GetDatabaseNamesCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if(!response)
		{
			throw_invalid_response();
		}

		_result = std::make_shared<ResultType>();
		if (!impl::utils::json_utils::get_val_from_json(*response, "Databases", *_result))
		{
			throw_invalid_response();
		}
	}

	bool GetDatabaseNamesOperation::GetDatabaseNamesCommand::is_read_request() const
	{
		return true;
	}
}