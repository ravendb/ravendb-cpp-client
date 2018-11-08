#pragma once

#include "types.h"
#include "RavenCommand.h"

namespace ravendb
{
	class GetDatabaseRecordCommand : public RavenCommand<DatabaseRecordWithEtag>
	{
	private:
		//DocumentConventions _conventions;
		std::string _database;

	public:

		~GetDatabaseRecordCommand() override = default;
		GetDatabaseRecordCommand(/*DocumentConventions conventions,*/ std::string database)
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
