#pragma once
#include "RavenCommand.h"

namespace ravendb::client::documents::conventions
{
		class DocumentConventions;
}

namespace ravendb::client::serverwide::operations
{
	template<typename TResult>
	class IServerOperation
	{
	public:
		virtual ~IServerOperation() = 0;

		//using std::unique_ptr for polymorphism
		virtual std::unique_ptr<http::RavenCommand<TResult>> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) const = 0;
	};

	template <typename TResult>
	IServerOperation<TResult>::~IServerOperation() = default;
}
