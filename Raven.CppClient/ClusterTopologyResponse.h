#pragma once
#include "json_utils.h"
#include "ClusterTopology.h"

namespace ravendb::client::http
{
	struct ClusterTopologyResponse
	{
		std::string leader{};
		std::string node_tag{};

		ClusterTopology topology{};
	};

	void from_json(const nlohmann::json& j, ClusterTopologyResponse& ctr);
}
