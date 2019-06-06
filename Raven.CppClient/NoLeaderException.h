#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::cluster
{
	class NoLeaderException : public RavenException
	{
	public:
		~NoLeaderException() override = default;

		NoLeaderException();

		explicit NoLeaderException(const std::string& message);
	};
}
