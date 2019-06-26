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
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class DeleteDatabaseCommand : public http::RavenCommand<DeleteDatabaseResult>
		{
		private:
			std::istringstream _parameters_stream;

		public:
			~DeleteDatabaseCommand() override;

			DeleteDatabaseCommand(std::shared_ptr<documents::conventions::DocumentConventions> conventions, const delete_db_op::Parameters& parameters);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
