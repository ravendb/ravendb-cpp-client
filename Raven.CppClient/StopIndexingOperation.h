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
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class StopIndexingCommand : public http::VoidRavenCommand
		{
		public:
			~StopIndexingCommand() override = default;

			StopIndexingCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}

