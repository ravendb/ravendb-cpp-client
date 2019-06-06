#pragma once
#include "RavenException.h"

namespace ravendb::client::http
{
	struct Topology;
}

namespace ravendb::client::exceptions
{
	class AllTopologyNodesDownException : public RavenException
	{
	private:
		const std::shared_ptr<const http::Topology> failed_topology{};

	public:
		~AllTopologyNodesDownException() override = default;

		std::shared_ptr<const http::Topology> get_failed_topology() const;

		AllTopologyNodesDownException();

		explicit AllTopologyNodesDownException(const std::string& message);

		AllTopologyNodesDownException(const std::string& message, std::shared_ptr<const http::Topology> failed_topology);
	};
}
