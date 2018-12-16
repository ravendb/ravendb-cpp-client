#pragma once
#include "IMaintenanceOperation.h"
#include "VoidRavenCommand.h"

using
	ravendb::client::http::VoidRavenCommandBase,
	ravendb::client::documents::conventions::DocumentConventions;

namespace ravendb::client::documents::operations
{
	using IVoidMaintenanceOperationBase = IMaintenanceOperation<void*>;

	struct IVoidMaintenanceOperation : IVoidMaintenanceOperationBase
	{
		~IVoidMaintenanceOperation() override = 0;

		std::unique_ptr<VoidRavenCommandBase> get_command(const DocumentConventions& conventions) const override = 0;
	};

	inline IVoidMaintenanceOperation::~IVoidMaintenanceOperation() = default;
}
