#pragma once
#include "stdafx.h"
#include "IVoidMaintenanceOperation.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode,
ravendb::client::http::VoidRavenCommand;

namespace ravendb::client::documents::operations::indexes
{
	class StartIndexingOperation : public IVoidMaintenanceOperation
	{
	public:
		~StartIndexingOperation() override = default;

		StartIndexingOperation() = default;

		std::unique_ptr<VoidRavenCommandBase> get_command(const DocumentConventions& conventions) override
		{
			return std::make_unique<StartIndexingCommand>();
		}

	private:
		class StartIndexingCommand : public VoidRavenCommand
		{
		public:
			~StartIndexingCommand() override = default;

			StartIndexingCommand() = default;

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases/" << node.database
					<< "/admin/indexes/start";

				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "");

				url = pathBuilder.str();
			}
		};
	};
}

