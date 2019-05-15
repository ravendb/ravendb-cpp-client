#include "stdafx.h"
#include "NodeIsPassiveException.h"

namespace ravendb::client::exceptions::cluster
{
	NodeIsPassiveException::NodeIsPassiveException() = default;

	NodeIsPassiveException::NodeIsPassiveException(const std::string& message)
		: RavenException(message)
	{}
}
