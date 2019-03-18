#pragma once
#include "IMaintenanceOperation.h"
#include "VoidRavenCommand.h"

namespace ravendb::client::documents::operations
{
	using IVoidMaintenanceOperationBase = IMaintenanceOperation<void>;

	struct IVoidMaintenanceOperation : IVoidMaintenanceOperationBase
	{
		~IVoidMaintenanceOperation() override = 0;

		std::unique_ptr<http::VoidRavenCommandBase> get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const override = 0;
	};

	inline IVoidMaintenanceOperation::~IVoidMaintenanceOperation() = default;
}
