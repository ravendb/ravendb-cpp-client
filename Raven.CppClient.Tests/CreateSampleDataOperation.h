#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::tests::infrastructure
{
	class CreateSampleDataOperation : documents::operations::IVoidMaintenanceOperation
	{
	public:
		std::unique_ptr<VoidRavenCommandBase> get_command(const DocumentConventions& conventions) const override;

	private:
		class CreateSampleDataCommand : public http::VoidRavenCommand
		{
		public:
			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;
		};
	};
}
