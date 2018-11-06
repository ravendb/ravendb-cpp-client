#pragma once
#include "RavenCommand.h"
#include "types.h"

namespace ravenDB
{
	using namespace std::chrono_literals;
	
	using TimeToWaitForConfirmation_t = std::chrono::duration<double>;//which units should we use ?

	class DeleteDatabaseCommand : public RavenCommand<DeleteDatabaseResult>
	{
	public:
		struct Parameters
		{
			std::vector<std::string> database_names{};
			bool hard_delete = true;
			std::vector<std::string> from_nodes{};
			TimeToWaitForConfirmation_t time_to_wait_for_confirmation{};
		};

	private:
		Parameters _parameters{};
		nlohmann::json _parameters_json{};

	public:

		~DeleteDatabaseCommand() override = default;

		DeleteDatabaseCommand(std::string database_name, bool hard_delete, std::string from_node = {},
			TimeToWaitForConfirmation_t time_to_wait_for_confirmation = 0s)
		{
			if (database_name.empty())
			{
				throw std::invalid_argument("database name can't be empty");
			}
			_parameters.database_names.push_back(std::move(database_name));
			_parameters.hard_delete = hard_delete;
			if (not from_node.empty())
			{
				_parameters.from_nodes.push_back(std::move(from_node));
			}
			_parameters.time_to_wait_for_confirmation = time_to_wait_for_confirmation;

			_parameters_json = _parameters;
		}

		DeleteDatabaseCommand(Parameters parameters)
			: _parameters(std::move(parameters))
			, _parameters_json(_parameters)
		{}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			//std::ostringstream pathBuilder;
			//pathBuilder << node.url << "/databases/" << node.database
			//	<< "/docs?id=" << ravenDB::impl::Utils::url_escape(curl, _id);

			//curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

			//if (not _change_vector.empty())
			//{
			//	std::ostringstream change_vector_header;
			//	change_vector_header << "If-Match:" << '"' << _change_vector << '"';

			//	_headers_list = curl_slist_append(_headers_list, change_vector_header.str().c_str());
			//	if (_headers_list == nullptr)
			//	{
			//		throw std::runtime_error("error in curl_slist_append");
			//	}
			//}
			//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list);

			//url = pathBuilder.str();
		}


		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			//_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return false;
		}
	};


	void to_json(const nlohmann::json& j, DeleteDatabaseCommand::Parameters& p);

}
