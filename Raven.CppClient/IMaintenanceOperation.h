#pragma once
#include "RavenCommand.h"
#include "DocumentConventions.h"

namespace ravendb::client::documents::operations
{
	template<typename TResult>
	struct IMaintenanceOperation
	{
		virtual ~IMaintenanceOperation() = 0;

		//using std::unique_ptr for polymorphism
		virtual std::unique_ptr<http::RavenCommand<TResult>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const = 0;
	};

	template <typename TResult>
	IMaintenanceOperation<TResult>::~IMaintenanceOperation() = default;
}
