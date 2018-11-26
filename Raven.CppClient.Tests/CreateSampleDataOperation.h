#pragma once

#include "stdafx.h"
#include "VoidRavenCommand.h"

namespace ravendb::client::tests::infrastructure
{

	class CreateSampleDataCommand : public http::VoidRavenCommand
	{
	public:

		void create_request(CURL* curl, const http::ServerNode& node, std::string& url) const override
		{
			std::ostringstream urlBuilder;
			urlBuilder << node.url << "/databases/" << node.database << "/studio/sample-data";

			curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "");

			url = urlBuilder.str();
		}
	};
}
