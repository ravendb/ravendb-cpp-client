#pragma once
#include "RavenCommand.h"
#include "DocumentConventions.h"

namespace ravendb::client::documents::operations
{
	using
		ravendb::client::http::RavenCommand,
		ravendb::client::documents::conventions::DocumentConventions;

	template<typename TResult>
	struct IMaintenanceOperation
	{
		virtual ~IMaintenanceOperation() = 0;

		//using std::unique_ptr for polymorphism
		virtual std::unique_ptr<RavenCommand<TResult>> get_command(const DocumentConventions& conventions) = 0;
	};

	template <typename TResult>
	IMaintenanceOperation<TResult>::~IMaintenanceOperation() = default;
}
