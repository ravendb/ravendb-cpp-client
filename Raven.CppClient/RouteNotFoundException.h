#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::routing
{
	class RouteNotFoundException : public RavenException
	{
	public:
		~RouteNotFoundException() override = default;

		RouteNotFoundException();

		explicit RouteNotFoundException(const std::string& message);
	};
}
