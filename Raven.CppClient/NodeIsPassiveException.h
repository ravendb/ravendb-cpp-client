#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::cluster
{
	class NodeIsPassiveException : public RavenException
	{
	public:
		~NodeIsPassiveException() override = default;

		NodeIsPassiveException();

		explicit NodeIsPassiveException(const std::string& message);
	};
}
