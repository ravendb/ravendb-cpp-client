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

	void GetDatabaseNamesOperation::GetDatabaseNamesCommand::create_request(CURL * curl, const http::ServerNode & node, std::string & url)
	{
		std::ostringstream pathBuilder;
		pathBuilder << node.url << "/databases?start=" << _start
			<< "&pageSize=" << _page_size << "&namesOnly=true";

		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

		url = pathBuilder.str();
	}

	void GetDatabaseNamesOperation::GetDatabaseNamesCommand::set_response(CURL * curl, const nlohmann::json & response, bool from_cache)
	{
		if (!impl::utils::json_utils::get_val_from_json(response, "Databases", _result))
		{
			throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::INVALID_RESPONSE);
		}
	}

	bool GetDatabaseNamesOperation::GetDatabaseNamesCommand::is_read_request() const noexcept
	{
		return true;
	}
}