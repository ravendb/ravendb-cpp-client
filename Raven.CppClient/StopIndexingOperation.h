#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	class StopIndexingOperation : public IVoidMaintenanceOperation
	{
	public:
		~StopIndexingOperation() override = default;

		StopIndexingOperation() = default;

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<StopIndexingCommand>();
		}

	private:
		class StopIndexingCommand : public http::VoidRavenCommand
		{
		public:
			~StopIndexingCommand() override = default;

			StopIndexingCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/admin/indexes/stop";

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPPOST, 1);
				curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, "");
				curl_ref.method = constants::methods::POST;

				url_ref.emplace(path_builder.str());
			}
		};
	};
}

