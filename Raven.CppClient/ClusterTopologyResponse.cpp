#include "stdafx.h"
#include "ClusterTopologyResponse.h"

namespace ravendb::client::http
{
	void from_json(const nlohmann::json& j, ClusterTopologyResponse& ctr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Leader", ctr.leader);
		get_val_from_json(j, "NodeTag", ctr.node_tag);
		get_val_from_json(j, "Topology", ctr.topology);
	}
}
