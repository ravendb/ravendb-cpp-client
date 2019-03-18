#include "pch.h"
#include "CreateSampleDataOperation.h"

namespace ravendb::client::tests::infrastructure
{
	std::unique_ptr<http::VoidRavenCommandBase> CreateSampleDataOperation::get_command(std::shared_ptr<DocumentConventions> conventions) const
	{
		return std::make_unique<CreateSampleDataCommand>();
	}

	void CreateSampleDataOperation::CreateSampleDataCommand::create_request(CURL* curl, const http::ServerNode& node, std::string& url)
	{
		std::ostringstream urlBuilder;
		urlBuilder << node.url << "/databases/" << node.database << "/studio/sample-data";

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "");

		url = urlBuilder.str();
	}
}