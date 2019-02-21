#include "stdafx.h"
#include "Topology.h"
#include "ServerNode.h"
#include "json_utils.h"

namespace ravendb::client::http
{
	void from_json(const nlohmann::json & j, Topology & top)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Etag", top.etag);
		if (!get_val_from_json(j, "Nodes", top.nodes))
		{
			throw std::invalid_argument("\"Nodes\" is absent or isn't an array");
		}
	}
}