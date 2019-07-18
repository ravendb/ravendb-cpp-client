#include "stdafx.h"
#include "DeleteIndexOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	DeleteIndexOperation::DeleteIndexOperation(std::string index_name)
		: _index_name([&]
	{
		if (index_name.empty())
		{
			throw std::invalid_argument("Index name must have a non empty value");
		}
		else
		{
			return std::move(index_name);
		}
	}())
	{}

	std::unique_ptr<http::VoidRavenCommandBase> DeleteIndexOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<DeleteIndexCommand>(_index_name);
	}

	DeleteIndexOperation::DeleteIndexCommand::DeleteIndexCommand(std::string index_name)
		: _index_name([&]
	{
		if (index_name.empty())
		{
			throw std::invalid_argument("Index name must have a non empty value");
		}
		else
		{
			return std::move(index_name);
		}
	}())
	{}

	void DeleteIndexOperation::DeleteIndexCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes?name=" << http::url_encode(curl_ref, _index_name);

		curl_easy_setopt(curl_ref.get(), CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_ref.method = constants::methods::DELETE_;

		url_ref.emplace(path_builder.str());
	}
}
