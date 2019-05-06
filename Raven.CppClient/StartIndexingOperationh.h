#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	class StartIndexingOperation : public IVoidMaintenanceOperation
	{
	public:
		~StartIndexingOperation() override = default;

		StartIndexingOperation() = default;

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<StartIndexingCommand>();
		}

	private:
		class StartIndexingCommand : public http::VoidRavenCommand
		{
		public:
			~StartIndexingCommand() override = default;

			StartIndexingCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/admin/indexes/start";

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPPOST, 1);
				curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, "");
				curl_ref.method = constants::methods::POST;

				url_ref.emplace(path_builder.str());
			}
		};
	};
}

