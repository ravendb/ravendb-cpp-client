#include "stdafx.h"
#include "ExplainQueryCommand.h"
#include "json_utils.h"

namespace ravendb::client::documents::commands
{
	void explain_query_command::from_json(const nlohmann::json& j, ExplainQueryResult& eqr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Index", eqr.index);
		get_val_from_json(j, "Reason", eqr.reason);
	}

	ExplainQueryCommand::ExplainQueryCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
		queries::IndexQuery index_query)
		: _conventions([&]
	{
		if(!conventions)
		{
			throw std::invalid_argument("Conventions cannot be empty");
		}
		return conventions;
	}())
		, _index_query(std::move(index_query))
	{}

	void ExplainQueryCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database
			<< "/queries?debug=explain";

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_ref.method = constants::methods::POST;

		auto&& json_str = nlohmann::json(_index_query).dump();

		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(json_str.size()));
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());

		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		url_ref.emplace(path_builder.str());
	}

	void ExplainQueryCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			_result.reset();
			return;
		}

		_result = std::make_shared<ResultType>();
		if(!impl::utils::json_utils::get_val_from_json(*response, "Results", *_result))
		{
			throw_invalid_response();
		}
	}

	bool ExplainQueryCommand::is_read_request() const
	{
		return true;
	}
}
