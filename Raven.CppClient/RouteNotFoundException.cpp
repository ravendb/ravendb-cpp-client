#include "stdafx.h"
#include "RouteNotFoundException.h"

namespace ravendb::client::exceptions::routing
{
	RouteNotFoundException::RouteNotFoundException() = default;

	RouteNotFoundException::RouteNotFoundException(const std::string& message)
		: RavenException(message)
	{}
}
