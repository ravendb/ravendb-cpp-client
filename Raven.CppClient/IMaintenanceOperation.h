#pragma once
#include "RavenCommand.h"
#include "DocumentConventions.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::documents::conventions::DocumentConventions;

namespace ravendb::client::documents::operations
{
	template<typename TResult>
	struct IMaintenanceOperation
	{
		virtual ~IMaintenanceOperation() = 0;

		//using std::unique_ptr for polymorphism
		virtual std::unique_ptr<RavenCommand<TResult>> get_command(std::shared_ptr<DocumentConventions> conventions) const = 0;
	};

	template <typename TResult>
	IMaintenanceOperation<TResult>::~IMaintenanceOperation() = default;
}
