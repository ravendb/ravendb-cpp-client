#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::indexes
{
	class IndexInvalidException : public RavenException
	{
	public:
		~IndexInvalidException() override = default;

		IndexInvalidException();

		explicit IndexInvalidException(const std::string& message);
	};
}
