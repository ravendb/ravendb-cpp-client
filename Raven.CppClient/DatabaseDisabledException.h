#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::database
{
	class DatabaseDisabledException : public RavenException
	{
	public:
		~DatabaseDisabledException() override = default;

		DatabaseDisabledException();

		explicit DatabaseDisabledException(const std::string& message);
	};
}
