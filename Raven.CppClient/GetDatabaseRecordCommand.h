#pragma once

#include "types.h"
#include "RavenCommand.h"

namespace ravendb::client
{
	using http::RavenCommand, http::ServerNode;


	class GetDatabaseRecordCommand : public RavenCommand<DatabaseRecordWithEtag>
	{
	private:
		std::string _database;

	public:

		~GetDatabaseRecordCommand() override = default;
		GetDatabaseRecordCommand(std::string database)
			: _database(std::move(database))
		{}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			std::ostringstream urlBuilder;
			urlBuilder << node.url << "/admin/databases?name=" << _database;

			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			url = urlBuilder.str();
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			long statusCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
			if (statusCode == 200)
				_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return false;//why ?
		}
	};
}