#include "stdafx.h"
#include "ServerNode.h"
#include "json_utils.h"

namespace ravendb::client::http
{
	ServerNode::ServerNode(std::string url, std::string db, std::string tag, Role role)
		: url(std::move(url))
		, database(std::move(db))
		, cluster_tag(std::move(tag))
		, server_role(role)
	{}

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

	bool CompareSharedPtrConstServerNode::operator()(
		std::shared_ptr<const ServerNode> lhs,
		std::shared_ptr<const ServerNode> rhs) const
	{
		return lhs ? (rhs ? *lhs == *rhs : false) : !rhs;
	}

	bool operator==(const ServerNode& lhs, const ServerNode& rhs)
	{
		return lhs.url == rhs.url
			&& lhs.database == rhs.database;
	}

	bool operator!=(const ServerNode& lhs, const ServerNode& rhs)
	{
		return !(lhs == rhs);
	}
}
