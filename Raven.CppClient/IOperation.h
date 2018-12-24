#pragma once
#include "RavenCommand.h"
#include "IDocumentStore.h"
#include "DocumentConventions.h"
#include "HttpCache.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::documents::IDocumentStore,
ravendb::client::documents::conventions::DocumentConventions,
ravendb::client::http::HttpCache;

namespace ravendb::client::documents::operations
{
	template<typename TResult>
	struct IOperation
	{
		virtual ~IOperation() = 0;

		//using std::unique_ptr for polymorphism
		virtual std::unique_ptr<RavenCommand<TResult>> get_command
			(const IDocumentStore& store, const DocumentConventions& conventions, HttpCache& cache) const = 0;
	};

	template <typename TResult>
	IOperation<TResult>::~IOperation() = default;
}
