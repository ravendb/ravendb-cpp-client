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
			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;
		};
	};
}
