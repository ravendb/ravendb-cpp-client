#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::session
{
	class NonUniqueObjectException : public RavenException
	{
	public:
		~NonUniqueObjectException() override = default;

		NonUniqueObjectException();

		explicit NonUniqueObjectException(const std::string& message);
	};
}
