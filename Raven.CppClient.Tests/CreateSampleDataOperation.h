#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::tests::infrastructure
{
	class CreateSampleDataOperation : documents::operations::IVoidMaintenanceOperation
	{
	public:
		std::unique_ptr<VoidRavenCommandBase> get_command(const DocumentConventions& conventions) const override
		{
			return std::make_unique<CreateSampleDataCommand>();
		}

	private:
		class CreateSampleDataCommand : public http::VoidRavenCommand
		{
		public:
			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override
			{
				std::ostringstream urlBuilder;
				urlBuilder << node.url << "/databases/" << node.database << "/studio/sample-data";

				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "");

				url = urlBuilder.str();
			}
		};
	};
}
