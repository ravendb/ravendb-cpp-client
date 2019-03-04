#include "stdafx.h"
#include "GetDatabaseRecordOperation.h"

namespace ravendb::client::serverwide::operations
{
	GetDatabaseRecordOperation::~GetDatabaseRecordOperation() = default;

	GetDatabaseRecordOperation::GetDatabaseRecordOperation(std::string database)
		: _database([&]
	{
		if (database.empty())
		{
			throw std::invalid_argument("database name can't be empty");
		}
		else
		{
			return std::move(database);
		}
	}())
	{}

	std::unique_ptr<http::RavenCommand<DatabaseRecord>> GetDatabaseRecordOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions)
	{
		return std::make_unique<GetDatabaseRecordCommand>(conventions, _database);
	}

	GetDatabaseRecordOperation::GetDatabaseRecordCommand::~GetDatabaseRecordCommand() = default;

	GetDatabaseRecordOperation::GetDatabaseRecordCommand::GetDatabaseRecordCommand(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions, std::string database)
		: _database(std::move(database))
	{}

	void GetDatabaseRecordOperation::GetDatabaseRecordCommand::create_request(
		CURL * curl, const http::ServerNode & node, std::string & url)
	{
		std::ostringstream urlBuilder;
		urlBuilder << node.url << "/admin/databases?name=" << _database;

		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

		url = urlBuilder.str();
	}

	void GetDatabaseRecordOperation::GetDatabaseRecordCommand::set_response(CURL * curl, const nlohmann::json & response, bool from_cache)
	{
		_result = response.get<decltype(_result)>();
	}

	bool GetDatabaseRecordOperation::GetDatabaseRecordCommand::is_read_request() const noexcept
	{
		return false;
	}
}