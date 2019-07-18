#include "stdafx.h"
#include "DisableIndexOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	DisableIndexOperation::DisableIndexOperation(std::string index_name)
		: _index_name([&]
	{
		if (index_name.empty())
		{
			throw std::invalid_argument("Index name must have a non empty value");
		}
		return std::move(index_name);
	}())
	{}

	std::unique_ptr<http::VoidRavenCommandBase> DisableIndexOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<DisableIndexCommand>(_index_name);
	}

	DisableIndexOperation::DisableIndexCommand::DisableIndexCommand(std::string index_name)
		: _index_name([&]
	{
		if (index_name.empty())
		{
			throw std::invalid_argument("Index name must have a non empty value");
		}
		return std::move(index_name);
	}())
	{}

	void DisableIndexOperation::DisableIndexCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/admin/indexes/disable?name=" << http::url_encode(curl_ref, _index_name);

		curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE, 0);
		curl_ref.method = constants::methods::POST;

		url_ref.emplace(path_builder.str());
	}
}
