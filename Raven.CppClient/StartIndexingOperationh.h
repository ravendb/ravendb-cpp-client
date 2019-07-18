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
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class StartIndexingCommand : public http::VoidRavenCommand
		{
		public:
			~StartIndexingCommand() override = default;

			StartIndexingCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}

