#pragma once
#include "RavenCommand.h"
#include "DocumentConventions.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::documents::conventions::DocumentConventions;

namespace ravendb::client::serverwide::operations
{
	template<typename TResult>
	struct IServerOperation
	{
		virtual ~IServerOperation() = 0;

		//using std::unique_ptr for polymorphism
		virtual std::unique_ptr<RavenCommand<TResult>> get_command(const DocumentConventions& conventions) = 0;
	};

	template <typename TResult>
	IServerOperation<TResult>::~IServerOperation() = default;
}
