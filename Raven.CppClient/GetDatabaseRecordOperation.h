#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "DatabaseRecord.h"
#include "IServerOperation.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode;

namespace ravendb::client::serverwide::operations
{
	class GetDatabaseRecordOperation : public IServerOperation<DatabaseRecord>
	{
	private:
		const std::string _database;

	public:
		~GetDatabaseRecordOperation() override = default;

		GetDatabaseRecordOperation(std::string database)
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

		std::unique_ptr<RavenCommand<DatabaseRecord>> get_command(const DocumentConventions& conventions) override
		{
			return std::make_unique<GetDatabaseRecordCommand>(conventions, _database);
		}

	private:
		class GetDatabaseRecordCommand : public RavenCommand<DatabaseRecord>
		{
		private:
			const std::string _database;

		public:
			~GetDatabaseRecordCommand() override = default;

			GetDatabaseRecordCommand(const DocumentConventions& conventions, std::string database)
				: _database(std::move(database))
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream urlBuilder;
				urlBuilder << node.url << "/admin/databases?name=" << _database;

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = urlBuilder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				_result = response;
			}

			bool is_read_request() const noexcept override
			{
				return false;
			}
		};
	};
}
