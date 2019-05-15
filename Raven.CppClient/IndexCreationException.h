#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	class IndexCreationException : public RavenException
	{
	public:
		~IndexCreationException() override = default;

		IndexCreationException();

		explicit IndexCreationException(const std::string& message);
	};
}
