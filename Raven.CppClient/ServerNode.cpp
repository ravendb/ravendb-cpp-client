#include "stdafx.h"
#include "ServerNode.h"
#include "json_utils.h"

namespace ravendb::client::http
{
	void from_json(const nlohmann::json & j, ServerNode & sn)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Url", sn.url);
		get_val_from_json(j, "Database", sn.database);
		get_val_from_json(j, "ClusterTag", sn.cluster_tag);
		get_val_from_json(j, "ServerRole", sn.server_role);
	}

	void to_json(nlohmann::json& j, const ServerNode& sn)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Url", sn.url);
		set_val_to_json(j, "Database", sn.database);
		set_val_to_json(j, "ClusterTag", sn.cluster_tag);
		set_val_to_json(j, "ServerRole", sn.server_role);
	}
}
