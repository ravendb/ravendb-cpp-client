#pragma once
#include "RavenCommand.h"
#include "DeleteDatabaseResult.h"
#include "IServerOperation.h"

namespace ravendb::client::serverwide::operations
{
	namespace delete_db_op
	{
		struct Parameters
		{
			static const std::chrono::milliseconds default_wait_time;

			std::vector<std::string> database_names{};
			bool hard_delete = true;
			std::vector<std::string> from_nodes{};
			std::chrono::milliseconds time_to_wait_for_confirmation = default_wait_time;
		};

		void to_json(nlohmann::json& j, const Parameters& dbp);
	}

	class DeleteDatabasesOperation : public IServerOperation<DeleteDatabaseResult>
	{
	private:
		delete_db_op::Parameters _parameters{};

	public:
		~DeleteDatabasesOperation() override;

		DeleteDatabasesOperation(std::string database_name, bool hard_delete, std::string from_node = {},
			std::chrono::milliseconds time_to_wait_for_confirmation = delete_db_op::Parameters::default_wait_time);

		explicit DeleteDatabasesOperation(delete_db_op::Parameters parameters);

		std::unique_ptr<http::RavenCommand<DeleteDatabaseResult>> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) override;

	private:
		class DeleteDatabaseCommand : public http::RavenCommand<DeleteDatabaseResult>
		{
		private:
			std::string _parameters_str{};

		public:
			~DeleteDatabaseCommand() override;

			DeleteDatabaseCommand(std::shared_ptr<documents::conventions::DocumentConventions> conventions, const delete_db_op::Parameters& parameters);

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override;

			bool is_read_request() const noexcept override;
		};
	};
}
