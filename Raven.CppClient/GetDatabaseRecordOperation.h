#pragma once
#include "RavenCommand.h"
#include "DatabaseRecord.h"
#include "IServerOperation.h"

namespace ravendb::client::serverwide::operations
{
	class GetDatabaseRecordOperation : public IServerOperation<DatabaseRecord>
	{
	private:
		const std::string _database;

	public:
		~GetDatabaseRecordOperation() override;

		GetDatabaseRecordOperation(std::string database);

		std::unique_ptr<http::RavenCommand<DatabaseRecord>> get_command(
			const documents::conventions::DocumentConventions& conventions) override;

	private:
		class GetDatabaseRecordCommand : public http::RavenCommand<DatabaseRecord>
		{
		private:
			const std::string _database;

		public:
			~GetDatabaseRecordCommand() override;

			GetDatabaseRecordCommand(const documents::conventions::DocumentConventions& conventions, std::string database);

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override;

			bool is_read_request() const noexcept override;
		};
	};
}
