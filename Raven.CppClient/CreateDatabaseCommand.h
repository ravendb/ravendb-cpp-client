#pragma once

#include "stdafx.h"
#include "RavenCommand.h"
#include "DatabasePutResult.h"
#include "DatabaseRecord.h"
#include "utils.h"

namespace ravendb::client::serverwide::operations
{
	using  ravendb::client::http::RavenCommand,
		ravendb::client::http::ServerNode;

	class CreateDatabaseCommand : public RavenCommand<DatabasePutResult>
	{
	private:
		std::unique_ptr<DatabaseRecord> _database_record;//polymorphism is needed
		int32_t _replication_factor;
		std::string _database_name;
		std::string _database_document;

	public:

		~CreateDatabaseCommand() override = default;

		CreateDatabaseCommand(const DatabaseRecord& database_record, int32_t replication_factor)
			: _database_record(std::make_unique<DatabaseRecord>(database_record))
			, _replication_factor(replication_factor)
			, _database_name([this]()->const std::string&
			{
				if (_database_record->database_name.empty()) throw std::invalid_argument("Database name is required");
				return _database_record->database_name;
			}())
			, _database_document(nlohmann::json(*_database_record).dump())
		{}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
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
}
