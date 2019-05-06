#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::tests::infrastructure
{
	class CreateSampleDataOperation : public documents::operations::IVoidMaintenanceOperation
	{
	public:
		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class CreateSampleDataCommand : public http::VoidRavenCommand
		{
		public:
			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}
