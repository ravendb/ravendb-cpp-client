#pragma once
#include "SpatialOptions.h"
#include "AutoSpatialMethodType.h"

namespace ravendb::client::documents::indexes::spatial
{
	struct AutoSpatialOptions : SpatialOptions
	{
		AutoSpatialMethodType method_type{};
		std::vector<std::string> method_arguments{};
	};
}
