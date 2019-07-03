#pragma once
#include "IVoidServerOperation.h"
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "VoidRavenCommand.h"

namespace ravendb::client::http
{
	struct ServerNode;
}

namespace ravendb::client::serverwide::operations::configuration
{
	namespace reorder_database_members_op
	{
		struct Parameters
		{
			std::vector<std::string> members_order{};
		};

		void to_json(nlohmann::json& j, const Parameters& p);

	}

	class ReorderDatabaseMembersOperation : public IVoidServerOperation
	{
	private:
		const std::string _database;
		const reorder_database_members_op::Parameters _parameters;

	public:
		~ReorderDatabaseMembersOperation() override;

		ReorderDatabaseMembersOperation(std::string database, std::vector<std::string> order);

		std::unique_ptr<http::RavenCommand<void>> get_command(std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class ReorderDatabaseMembersCommand : public http::VoidRavenCommand
		{
		private:
			const std::string _database_name;
			const std::string _parameters_str;

		public:
			~ReorderDatabaseMembersCommand() override;

			ReorderDatabaseMembersCommand(std::string database_name, const reorder_database_members_op::Parameters& parameters);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}
