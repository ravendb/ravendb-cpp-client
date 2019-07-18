#include "stdafx.h"
#include "ToggleDatabasesStateOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	void toggle_databases_state_operation::to_json(nlohmann::json& j, const Parameters& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "DatabaseNames", p.database_names);
	}

	ToggleDatabasesStateOperation::~ToggleDatabasesStateOperation() = default;

	ToggleDatabasesStateOperation::ToggleDatabasesStateOperation(std::string database_name, bool disable)
		: _disabled(disable)
		, _parameters({{std::move(database_name)}})

	{}

	ToggleDatabasesStateOperation::ToggleDatabasesStateOperation(std::vector<std::string> database_names, bool disable)
		: _disabled(disable)
		, _parameters({std::move(database_names)})
	{}

	ToggleDatabasesStateOperation::ToggleDatabasesStateOperation(toggle_databases_state_operation::Parameters parameters,
		bool disable)
		: _disabled(disable)
		, _parameters(std::move(parameters))
	{}

	std::unique_ptr<http::RavenCommand<DisableDatabaseToggleResult>> ToggleDatabasesStateOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<ToggleDatabaseStateCommand>(conventions, _parameters, _disabled);
	}

	ToggleDatabasesStateOperation::ToggleDatabaseStateCommand::~ToggleDatabaseStateCommand() = default;

	ToggleDatabasesStateOperation::ToggleDatabaseStateCommand::ToggleDatabaseStateCommand(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		toggle_databases_state_operation::Parameters parameters, bool disabled)
		:  _disabled(disabled)
		, _parameters(std::move(parameters))
		, _parameters_json_str(nlohmann::json(_parameters).dump())
	{}

	void ToggleDatabasesStateOperation::ToggleDatabaseStateCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/admin/databases/" <<
			(_disabled ? "disable" : "enable");

		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");
		curl_ref.method = constants::methods::POST;

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(_parameters_json_str.size()));
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDS, _parameters_json_str.c_str());

		url_ref.emplace(path_builder.str());
	}

	void ToggleDatabasesStateOperation::ToggleDatabaseStateCommand::set_response(
		const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if(!response)
		{
			throw_invalid_response();
		}

		auto&& json_node = response.value();
		auto&& status_node = json_node.at("Status");

		if(!status_node.is_array())
		{
			throw_invalid_response();
		}

		auto&& database_status = status_node.at(0);
		_result = std::make_shared<ResultType>(database_status.get<ResultType>());

	}

	bool ToggleDatabasesStateOperation::ToggleDatabaseStateCommand::is_read_request() const
	{
		return false;
	}
}
