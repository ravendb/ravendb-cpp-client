#pragma once
#include "IServerOperation.h"
#include "DatabasePutResult.h"
#include "RavenCommand.h"

namespace ravendb::client::serverwide::operations
{
	class AddDatabaseNodeOperation : public IServerOperation<DatabasePutResult>
	{
	private:
		const std::string _database_name;
		const std::optional<std::string> _node;

	public:
		~AddDatabaseNodeOperation() override;

		explicit AddDatabaseNodeOperation(std::string database_name, std::optional<std::string> node = {});

		std::unique_ptr<http::RavenCommand<DatabasePutResult>> get_command(std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class AddDatabaseNodeCommand : public http::RavenCommand<DatabasePutResult>
		{
		private:
			const std::string _database_name;
			const std::optional<std::string> _node;

		public:
			~AddDatabaseNodeCommand() override;

			AddDatabaseNodeCommand(std::string database_name, std::optional<std::string> node);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
