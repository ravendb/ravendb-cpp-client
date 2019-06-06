#include "stdafx.h"
#include "ClusterTopology.h"

namespace ravendb::client::http
{
	bool ClusterTopology::contains(const std::string& node) const
	{
		return
			members.find(node) != members.end() ||
			promotables.find(node) != promotables.end() ||
			watchers.find(node) != watchers.end();
	}

	std::optional<std::string> ClusterTopology::get_url_from_tag(const std::optional<std::string>& tag) const
	{
		if(!tag)
		{
			return {};
		}

		if(auto it = members.find(*tag);
			it != members.end())
		{
			return it->second;
		}
		if (auto it = promotables.find(*tag);
			it != promotables.end())
		{
			return it->second;
		}
		if (auto it = watchers.find(*tag);
			it != watchers.end())
		{
			return it->second;
		}
		return {};
	}

	std::unordered_map<std::string, std::string> ClusterTopology::get_all_nodes() const
	{
		std::unordered_map<std::string, std::string> result{};

		std::copy(members.cbegin(), members.cend(), std::inserter(result, result.end()));
		std::copy(promotables.cbegin(), promotables.cend(), std::inserter(result, result.end()));
		std::copy(watchers.cbegin(), watchers.cend(), std::inserter(result, result.end()));

		return  std::move(result);
	}

	void from_json(const nlohmann::json& j, ClusterTopology& ct)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "LastNodeId", ct.last_node_id);
		get_val_from_json(j, "TopologyId", ct.topology_id);

		get_val_from_json(j, "Members", ct.members);
		get_val_from_json(j, "Promotables", ct.promotables);
		get_val_from_json(j, "Watchers", ct.watchers);
	}
}
