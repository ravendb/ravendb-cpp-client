#pragma once
#include "RavenCommand.h"
#include "IDocumentStore.h"
#include "DocumentConventions.h"
#include "HttpCache.h"

namespace ravendb::client::documents::operations
{
	template<typename TResult>
	struct IOperation
	{
		virtual ~IOperation() = 0;

		//using std::unique_ptr for polymorphism
		virtual std::unique_ptr<http::RavenCommand<TResult>> get_command
			(std::shared_ptr<IDocumentStore> store, std::shared_ptr<conventions::DocumentConventions> conventions, http::HttpCache& cache) const = 0;
	};

	template <typename TResult>
	IOperation<TResult>::~IOperation() = default;
}
