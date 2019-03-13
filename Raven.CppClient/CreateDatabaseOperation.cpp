#include "stdafx.h"
#include "CreateDatabaseOperation.h"
#include "ServerNode.h"
#include "DatabaseRecord.h"
#include "utils.h"

namespace ravendb::client::serverwide::operations
{
	CreateDatabaseOperation::~CreateDatabaseOperation() = default;

	CreateDatabaseOperation::CreateDatabaseOperation(DatabaseRecord database_record, int32_t replication_factor)
		: _database_record(std::move(database_record))
		, _replication_factor(replication_factor)
	{}

	std::unique_ptr<http::RavenCommand<DatabasePutResult>> CreateDatabaseOperation::get_command(
		const std::shared_ptr<documents::conventions::DocumentConventions> conventions)
	{
		return std::make_unique<CreateDatabaseCommand>(conventions, _database_record, _replication_factor);
	}

	CreateDatabaseOperation::CreateDatabaseCommand::~CreateDatabaseCommand() = default;

	CreateDatabaseOperation::CreateDatabaseCommand::CreateDatabaseCommand(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions, DatabaseRecord database_record, int32_t replication_factor)
		: _conventions(conventions)
		, _database_record(std::move(database_record))
		, _replication_factor(replication_factor)
		, _database_name([this]()-> const std::string&
	{
		if (_database_record.database_name.empty())
		{
			throw std::invalid_argument("Database name is required");
		}
		else
		{
			return _database_record.database_name;
		}
	}())
		, _database_document(nlohmann::json(_database_record).dump())
	{}

	void CreateDatabaseOperation::CreateDatabaseCommand::create_request(CURL* curl, const http::ServerNode& node, std::string& url)
	{
		std::ostringstream path_builder;
		path_builder << node.url << "/admin/databases?name=" << _database_name
			<< "&replicationFactor=" << _replication_factor;

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, ravendb::client::impl::utils::read_callback);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_PUT, 1L);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_database_document);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_database_document.length());

		url = path_builder.str();
	}

	void CreateDatabaseOperation::CreateDatabaseCommand::set_response(CURL* curl, const nlohmann::json& response, bool from_cache)
	{
		_result = response.get<decltype(_result)>();
	}

	bool CreateDatabaseOperation::CreateDatabaseCommand::is_read_request() const noexcept
	{
		return false;
	}

}