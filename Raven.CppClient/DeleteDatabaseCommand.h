#pragma once
#include "stdafx.h"
#include <iomanip>
#include "RavenCommand.h"
#include "DeleteDatabaseResult.h"
#include "utils.h"
#include "json_utils.h"


namespace ravendb::client::serverwide::operations
{
	struct DeleteDatabaseParameters //will be inside the operation
	{
		std::vector<std::string> database_names{};
		bool hard_delete = true;
		std::vector<std::string> from_nodes{};
		std::chrono::milliseconds time_to_wait_for_confirmation{};
	};

	inline void to_json(nlohmann::json& j, const DeleteDatabaseParameters& dbp)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "DatabaseNames", dbp.database_names);
		set_val_to_json(j, "FromNodes", dbp.from_nodes);
		set_val_to_json(j, "HardDelete", dbp.hard_delete);

		using namespace std::chrono;
		std::ostringstream time_dur;
		uint64_t h, m, s, ms;
		h = duration_cast<hours>(dbp.time_to_wait_for_confirmation).count();
		m = duration_cast<minutes>(dbp.time_to_wait_for_confirmation%hours(1)).count();
		s = duration_cast<seconds>(dbp.time_to_wait_for_confirmation%minutes(1)).count();
		ms = (dbp.time_to_wait_for_confirmation%seconds(1)).count();
		time_dur << std::setfill('0') <<
			std::setw(2) << h << ":" <<
			std::setw(2) << m << ":" <<
			std::setw(2) << s << "." <<
			std::setw(3) << ms;

		set_val_to_json(j, "TimeToWaitForConfirmation", time_dur.str());
	}

	class DeleteDatabaseCommand : public RavenCommand<DeleteDatabaseResult>
	{
	private:
		DeleteDatabaseParameters _parameters{};
		std::string _parameters_str{};

	public:

		inline static const std::chrono::milliseconds default_wait_time = std::chrono::seconds(5);

		~DeleteDatabaseCommand() override = default;

		DeleteDatabaseCommand(std::string database_name, bool hard_delete, std::string from_node = {},
			std::chrono::milliseconds time_to_wait_for_confirmation = default_wait_time)
		{
			if (database_name.empty())
			{
				throw std::invalid_argument("database name can't be empty");
			}
			_parameters.database_names.push_back(std::move(database_name));
			_parameters.hard_delete = hard_delete;
			if (! from_node.empty())
			{
				_parameters.from_nodes.push_back(std::move(from_node));
			}
			_parameters.time_to_wait_for_confirmation = time_to_wait_for_confirmation;

			_parameters_str = nlohmann::json(_parameters).dump();
		}

		DeleteDatabaseCommand(DeleteDatabaseParameters parameters)
			:_parameters(std::move(parameters))
		{
			if (_parameters.database_names.empty())
			{
				throw std::invalid_argument("database name can't be empty");
			}
			_parameters_str = nlohmann::json(_parameters).dump();
		}

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

}
