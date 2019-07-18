#include "stdafx.h"
#include "ReorderDatabaseMembersOperation.h"
#include "json_utils.h"

namespace ravendb::client::serverwide::operations::configuration
{
	namespace reorder_database_members_op
	{
		void to_json(nlohmann::json& j, const Parameters& p)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "MembersOrder", p.members_order);
		}
	}

	ReorderDatabaseMembersOperation::~ReorderDatabaseMembersOperation() = default;

	ReorderDatabaseMembersOperation::ReorderDatabaseMembersOperation(std::string database,
		std::vector<std::string> order)
		: _database(std::move(database))
		, _parameters({ [&]()
	{
		if (order.empty())
		{
			throw std::invalid_argument("Order list must contain values");
		}
		return std::move(order);
	}() })
	{}

	std::unique_ptr<http::RavenCommand<void>> ReorderDatabaseMembersOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<ReorderDatabaseMembersCommand>(_database, _parameters);
	}

	ReorderDatabaseMembersOperation::ReorderDatabaseMembersCommand::~ReorderDatabaseMembersCommand() = default;

	ReorderDatabaseMembersOperation::ReorderDatabaseMembersCommand::ReorderDatabaseMembersCommand(
		std::string database_name, const reorder_database_members_op::Parameters& parameters)
		: _database_name([&]()
		{
			if(database_name.empty())
			{
				throw std::invalid_argument("Database cannot be empty");
			}
			return std::move(database_name);
		}())
		, _parameters_str([&]()
	{
		nlohmann::json j = parameters;
		return j.dump();
	}())
	{}

	void ReorderDatabaseMembersOperation::ReorderDatabaseMembersCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/admin/databases/reorder?name=" << _database_name;

		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");
		curl_ref.method = constants::methods::POST;

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(_parameters_str.size()));
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDS, _parameters_str.c_str());

		url_ref.emplace(path_builder.str());
	}
}
