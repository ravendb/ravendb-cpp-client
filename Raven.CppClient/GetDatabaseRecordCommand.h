#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "DatabaseRecord.h"

namespace ravendb::client::serverwide::operations
{
	using ravendb::client::http::RavenCommand,
		ravendb::client::http::ServerNode;

	class GetDatabaseRecordCommand : public RavenCommand<DatabaseRecord>
	{
	private:
		std::string _database;

	public:

		~GetDatabaseRecordCommand() override = default;
		GetDatabaseRecordCommand(std::string database)
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
}
