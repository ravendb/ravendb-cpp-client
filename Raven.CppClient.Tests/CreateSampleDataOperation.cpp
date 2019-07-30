#include "pch.h"
#include "CreateSampleDataOperation.h"

namespace ravendb::client::tests::infrastructure
{
	std::unique_ptr<http::VoidRavenCommandBase> CreateSampleDataOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<CreateSampleDataCommand>();
	}

	void CreateSampleDataOperation::CreateSampleDataCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream url_builder{};
		url_builder << node->url << "/databases/" << node->database << "/studio/sample-data";

		curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE, 0);
		curl_ref.method = constants::methods::POST;

		url_ref.emplace(url_builder.str());
	}
}