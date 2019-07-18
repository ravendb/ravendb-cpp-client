#include "stdafx.h"
#include "StopIndexingOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	std::unique_ptr<http::VoidRavenCommandBase> StopIndexingOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<StopIndexingCommand>();
	}

	void StopIndexingOperation::StopIndexingCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/admin/indexes/stop";

		curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE, 0);
		curl_ref.method = constants::methods::POST;

		url_ref.emplace(path_builder.str());
	}
}
