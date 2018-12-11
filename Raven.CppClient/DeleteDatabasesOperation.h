#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "DeleteDatabaseResult.h"
#include "utils.h"
#include "json_utils.h"
#include "IServerOperation.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode;

namespace ravendb::client::serverwide::operations
{
	namespace delete_db_op
	{
		struct Parameters
		{
			inline static const std::chrono::milliseconds default_wait_time = std::chrono::seconds(5);

			std::vector<std::string> database_names{};
			bool hard_delete = true;
			std::vector<std::string> from_nodes{};
			std::chrono::milliseconds time_to_wait_for_confirmation = default_wait_time;
		};

		inline void to_json(nlohmann::json& j, const Parameters& dbp)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "DatabaseNames", dbp.database_names);
			set_val_to_json(j, "FromNodes", dbp.from_nodes);
			set_val_to_json(j, "HardDelete", dbp.hard_delete);
			set_val_to_json(j, "TimeToWaitForConfirmation", dbp.time_to_wait_for_confirmation);
		}
	}

	class DeleteDatabasesOperation : public IServerOperation<DeleteDatabaseResult>
	{
	private:
		delete_db_op::Parameters _parameters;

	public:
		~DeleteDatabasesOperation() override = default;
		
		DeleteDatabasesOperation(std::string database_name, bool hard_delete)
			: DeleteDatabasesOperation(std::move(database_name), hard_delete, {}, delete_db_op::Parameters::default_wait_time)
		{}

		DeleteDatabasesOperation(std::string database_name, bool hard_delete, std::string from_node)
			: DeleteDatabasesOperation(std::move(database_name), hard_delete, std::move(from_node),
				delete_db_op::Parameters::default_wait_time)
		{}

		DeleteDatabasesOperation(std::string database_name, bool hard_delete, std::string from_node,
			std::chrono::milliseconds time_to_wait_for_confirmation = delete_db_op::Parameters::default_wait_time)
		{
			if (database_name.empty())
			{
				throw std::invalid_argument("database name can't be empty");
			}
			_parameters.database_names.push_back(std::move(database_name));
			_parameters.hard_delete = hard_delete;
			if (!from_node.empty())
			{
				_parameters.from_nodes.push_back(std::move(from_node));
			}
			_parameters.time_to_wait_for_confirmation = time_to_wait_for_confirmation;
		}

		DeleteDatabasesOperation(delete_db_op::Parameters parameters)
			: _parameters(std::move(parameters))
		{
			if (_parameters.database_names.size() != 1 || _parameters.database_names[0].empty())
			{
				throw std::invalid_argument("there should be one nonempty database name");
			}
		}

		std::unique_ptr<RavenCommand<DeleteDatabaseResult>> get_command(const DocumentConventions& conventions) override
		{
			return std::make_unique<DeleteDatabaseCommand>(conventions, _parameters);
		}

	private:
		class DeleteDatabaseCommand : public RavenCommand<DeleteDatabaseResult>
		{
		private:
			std::string _parameters_str{};

		public:
			~DeleteDatabaseCommand() override = default;

			DeleteDatabaseCommand(const DocumentConventions& conventions, const delete_db_op::Parameters& parameters)
				:_parameters_str([&]
			{
				nlohmann::json j = parameters;
				return j.dump();
			}())
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/admin/databases";

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, impl::utils::read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_parameters_str);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_parameters_str.length());
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

				url = pathBuilder.str();
			}


			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				_result = response;
			}

			bool is_read_request() const noexcept override
			{
				return false;
			}
		};
	};
}
