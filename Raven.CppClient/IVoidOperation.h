#pragma once
#include "IOperation.h"

namespace ravendb::client::documents::operations
{
	//Represents operation which doesn't return any response
	class IVoidOperation : public IOperation<void>
	{
	public:
		~IVoidOperation() override = 0;

		std::unique_ptr<http::RavenCommand<void>> get_command(
			std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions,
			http::HttpCache& cache) const override = 0;
	};

	inline IVoidOperation::~IVoidOperation() = default;
}
