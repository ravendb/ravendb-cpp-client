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
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<GetDatabaseRecordCommand>(conventions, _database);
	}

	GetDatabaseRecordOperation::GetDatabaseRecordCommand::~GetDatabaseRecordCommand() = default;

	GetDatabaseRecordOperation::GetDatabaseRecordCommand::GetDatabaseRecordCommand(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions, std::string database)
		: _database(std::move(database))
	{}

	void GetDatabaseRecordOperation::GetDatabaseRecordCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream url_builder;
		url_builder << node->url << "/admin/databases?name=" << _database;

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(url_builder.str());
	}

	void GetDatabaseRecordOperation::GetDatabaseRecordCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if(!response)
		{
			_result.reset();
			return;
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool GetDatabaseRecordOperation::GetDatabaseRecordCommand::is_read_request() const
	{
		return false;
	}
}