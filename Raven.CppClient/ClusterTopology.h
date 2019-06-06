#pragma once
#include "json_utils.h"

namespace ravendb::client::http
{
	struct ClusterTopology
	{
		std::string last_node_id{};
		std::string topology_id{};

		std::unordered_map<std::string, std::string> members{};
		std::unordered_map<std::string, std::string> promotables{};
		std::unordered_map<std::string, std::string> watchers{};

		bool contains(const std::string& node) const;

		std::optional<std::string> get_url_from_tag(const std::optional<std::string>& tag) const;

		std::unordered_map<std::string, std::string> get_all_nodes() const;
	};

	void from_json(const nlohmann::json& j, ClusterTopology& ct);
}