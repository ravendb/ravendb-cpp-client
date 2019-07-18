#include "stdafx.h"
#include "StopIndexOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	StopIndexOperation::StopIndexOperation(std::string index_name)
		: _index_name([&]
	{
		if (index_name.empty())
		{
			throw std::invalid_argument("Index name must have a non empty value");
		}
		return std::move(index_name);
	}())
	{}

	std::unique_ptr<http::VoidRavenCommandBase> StopIndexOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<StopIndexCommand>(_index_name);
	}

	StopIndexOperation::StopIndexCommand::StopIndexCommand(std::string index_name)
		: _index_name([&]
	{
		if (index_name.empty())
		{
			throw std::invalid_argument("Index name must have a non empty value");
		}
		return std::move(index_name);
	}())
	{}

	void StopIndexOperation::StopIndexCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/admin/indexes/stop?name=" << http::url_encode(curl_ref, _index_name);

		curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE, 0);
		curl_ref.method = constants::methods::POST;

		url_ref.emplace(path_builder.str());
	}
}
