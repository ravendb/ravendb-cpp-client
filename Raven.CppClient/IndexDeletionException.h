#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	class IndexDeletionException : public RavenException
	{
	public:
		~IndexDeletionException() override = default;

		IndexDeletionException();

		explicit IndexDeletionException(const std::string& message);
	};
}
