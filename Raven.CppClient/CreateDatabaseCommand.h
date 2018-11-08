#pragma once

#include "stdafx.h"
#include "types.h"


#pragma once
#include "RavenCommand.h"

namespace ravendb
{
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

			curl_easy_setopt(curl, CURLOPT_READFUNCTION, ravendb::impl::Utils::read_callback);
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

	inline void to_json(nlohmann::json& j, const DatabaseRecord& dbr)
	{
		j["DatabaseName"] = dbr.database_name;
		j["Disabled"] = dbr.disabled;
		j["Settings"] = dbr.settings;
		//to be continued ...
	}
}
