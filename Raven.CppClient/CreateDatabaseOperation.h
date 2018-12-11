#pragma once

#include "stdafx.h"
#include "RavenCommand.h"
#include "DatabasePutResult.h"
#include "DatabaseRecord.h"
#include "utils.h"
#include "IServerOperation.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode;

namespace ravendb::client::serverwide::operations
{
	class CreateDatabaseOperation : public IServerOperation<DatabasePutResult>
	{
	private:
		const DatabaseRecord _database_record;
		const int32_t _replication_factor;

	public:
		~CreateDatabaseOperation() override = default;

		CreateDatabaseOperation(DatabaseRecord database_record, int32_t replication_factor = 1)
			: _database_record(std::move(database_record))
			, _replication_factor(replication_factor)
		{}

		std::unique_ptr<RavenCommand<DatabasePutResult>> get_command(const DocumentConventions& conventions) override
		{
			return std::make_unique<CreateDatabaseCommand>(conventions, _database_record, _replication_factor);
		}

	private:
		class CreateDatabaseCommand : public RavenCommand<DatabasePutResult>
		{
		private:
			const DocumentConventions _conventions;
			const DatabaseRecord _database_record;
			const int32_t _replication_factor;
			const std::string _database_name;
			const std::string _database_document;

		public:

			~CreateDatabaseCommand() override = default;

			CreateDatabaseCommand(const DocumentConventions& conventions, DatabaseRecord database_record, int32_t replication_factor)
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

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/admin/databases?name=" << _database_name
					<< "&replicationFactor=" << _replication_factor;

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, ravendb::client::impl::utils::read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_PUT, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_database_document);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_database_document.length());

				url = pathBuilder.str();
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
