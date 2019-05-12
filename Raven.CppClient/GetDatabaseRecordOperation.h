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
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) override;

	private:
		class GetDatabaseRecordCommand : public http::RavenCommand<DatabaseRecord>
		{
		private:
			const std::string _database;

		public:
			~GetDatabaseRecordCommand() override;

			GetDatabaseRecordCommand(std::shared_ptr<documents::conventions::DocumentConventions> conventions, std::string database);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
