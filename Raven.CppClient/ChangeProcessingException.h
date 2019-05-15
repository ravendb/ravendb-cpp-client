#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::changes
{
	class ChangeProcessingException : public RavenException
	{
	public:
		~ChangeProcessingException() override = default;

		ChangeProcessingException();

		explicit ChangeProcessingException(const std::string& message);
	};
}
