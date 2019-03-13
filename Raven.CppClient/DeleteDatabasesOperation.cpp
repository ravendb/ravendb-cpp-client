#include "stdafx.h"
#include "DeleteDatabasesOperation.h"
#include "json_utils.h"
#include "utils.h"

namespace ravendb::client::serverwide::operations
{
	namespace delete_db_op
	{
		const std::chrono::milliseconds Parameters::default_wait_time = std::chrono::seconds(5);

		void to_json(nlohmann::json& j, const Parameters& dbp)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "DatabaseNames", dbp.database_names);
			set_val_to_json(j, "FromNodes", dbp.from_nodes);
			set_val_to_json(j, "HardDelete", dbp.hard_delete);
			set_val_to_json(j, "TimeToWaitForConfirmation", dbp.time_to_wait_for_confirmation);
		}
	}

	DeleteDatabasesOperation::~DeleteDatabasesOperation() = default;

	DeleteDatabasesOperation::DeleteDatabasesOperation(std::string database_name, bool hard_delete, std::string from_node, std::chrono::milliseconds time_to_wait_for_confirmation)
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

	DeleteDatabasesOperation::DeleteDatabasesOperation(delete_db_op::Parameters parameters)
		: _parameters(std::move(parameters))
	{
		if (_parameters.database_names.size() != 1 || _parameters.database_names[0].empty())
		{
			throw std::invalid_argument("there should be one nonempty database name");
		}
	}

	std::unique_ptr<http::RavenCommand<DeleteDatabaseResult>> DeleteDatabasesOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions)
	{
		return std::make_unique<DeleteDatabaseCommand>(conventions, _parameters);
	}


	DeleteDatabasesOperation::DeleteDatabaseCommand::~DeleteDatabaseCommand() = default;

	DeleteDatabasesOperation::DeleteDatabaseCommand::DeleteDatabaseCommand(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions, const delete_db_op::Parameters & parameters)
		:_parameters_str([&]
	{
		nlohmann::json j = parameters;
		return j.dump();
	}())
	{}

	void DeleteDatabasesOperation::DeleteDatabaseCommand::create_request(CURL * curl, const http::ServerNode & node, std::string & url)
	{
		std::ostringstream path_builder;
		path_builder << node.url << "/admin/databases";

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, impl::utils::read_callback);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_parameters_str);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_parameters_str.length());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

		url = path_builder.str();
	}

	void DeleteDatabasesOperation::DeleteDatabaseCommand::set_response(CURL * curl, const nlohmann::json & response, bool from_cache)
	{
		_result = response.get<decltype(_result)>();
	}

	bool DeleteDatabasesOperation::DeleteDatabaseCommand::is_read_request() const noexcept
	{
		return false;
	}

}
