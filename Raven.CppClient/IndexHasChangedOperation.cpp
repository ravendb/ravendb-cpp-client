#include "stdafx.h"
#include "IndexHasChangedOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::indexes
{
	IndexHasChangedOperation::IndexHasChangedOperation(documents::indexes::IndexDefinition definition)
		: _index_definition(std::move(definition))
	{}

	std::unique_ptr<http::RavenCommand<bool>> IndexHasChangedOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<IndexHasChangedCommand>(conventions, _index_definition);
	}

	IndexHasChangedOperation::IndexHasChangedCommand::IndexHasChangedCommand(
		std::shared_ptr<conventions::DocumentConventions> conventions,
		const documents::indexes::IndexDefinition& definition)
		: _definition(definition)
	{}

	void IndexHasChangedOperation::IndexHasChangedCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes/has-changed";

		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);

		auto&& json_str = _definition.dump();

		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(json_str.size()));
		curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, json_str.c_str());

		curl_ref.method = constants::methods::POST;

		url_ref.emplace(path_builder.str());
	}

	void IndexHasChangedOperation::IndexHasChangedCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>();
		if (!impl::utils::json_utils::get_val_from_json(*response, "Changed", *_result))
		{
			throw_invalid_response();
		}
	}

	bool IndexHasChangedOperation::IndexHasChangedCommand::is_read_request() const
	{
		return false;
	}
}
