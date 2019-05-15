#include "stdafx.h"
#include "AllTopologyNodesDownException.h"

namespace ravendb::client::exceptions
{
	std::shared_ptr<const http::Topology> AllTopologyNodesDownException::get_failed_topology() const
	{
		return failed_topology;
	}

	AllTopologyNodesDownException::AllTopologyNodesDownException() = default;

	AllTopologyNodesDownException::AllTopologyNodesDownException(const std::string& message)
		:	RavenException(message)
	{}

	AllTopologyNodesDownException::AllTopologyNodesDownException(const std::string& message,
		std::shared_ptr<const http::Topology> failed_topology)
		: RavenException(message)
		, failed_topology(failed_topology)
	{}
}
