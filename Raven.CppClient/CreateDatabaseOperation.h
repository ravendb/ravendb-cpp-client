#pragma once
#include "IServerOperation.h"
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "DatabaseRecord.h"
#include "DatabasePutResult.h"

namespace ravendb::client
{
	namespace http
	{
		struct ServerNode;
	}
	namespace serverwide
	{
		struct DatabaseRecord;
	}
}

namespace ravendb::client::serverwide::operations
{
	class CreateDatabaseOperation : public IServerOperation<DatabasePutResult>
	{
	private:
		const DatabaseRecord _database_record;
		const int32_t _replication_factor;

	public:
		~CreateDatabaseOperation() override;

		CreateDatabaseOperation(DatabaseRecord database_record, int32_t replication_factor = 1);

		std::unique_ptr<http::RavenCommand<DatabasePutResult>> get_command(std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	public:
		class CreateDatabaseCommand : public http::RavenCommand<DatabasePutResult>
		{
		private:
			const std::shared_ptr<documents::conventions::DocumentConventions> _conventions;
			const DatabaseRecord _database_record;
			const int32_t _replication_factor;
			const std::optional<int64_t> _etag;
			const std::string _database_name;
			std::istringstream _database_document_stream;

		public:
			~CreateDatabaseCommand() override;

			CreateDatabaseCommand(std::shared_ptr<documents::conventions::DocumentConventions> conventions,
				DatabaseRecord database_record, int32_t replication_factor, std::optional<int64_t> etag = {});

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
