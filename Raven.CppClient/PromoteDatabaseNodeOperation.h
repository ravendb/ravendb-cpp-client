#pragma once
#include "IServerOperation.h"
#include "DatabasePutResult.h"
#include "RavenCommand.h"

namespace ravendb::client::serverwide::operations
{
	class PromoteDatabaseNodeOperation : public IServerOperation<DatabasePutResult>
	{
	private:
		const std::string _database_name;
		const std::string _node;

	public:
		~PromoteDatabaseNodeOperation() override;

		PromoteDatabaseNodeOperation(std::string database_name, std::string node = {});

		std::unique_ptr<http::RavenCommand<DatabasePutResult>> get_command(std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class PromoteDatabaseNodeCommand : public http::RavenCommand<DatabasePutResult>
		{
		private:
			const std::string _database_name;
			const std::string _node;

		public:
			~PromoteDatabaseNodeCommand() override;

			PromoteDatabaseNodeCommand(std::string database_name, std::string node);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
