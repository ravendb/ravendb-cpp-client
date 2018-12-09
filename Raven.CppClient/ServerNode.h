#pragma once
#include "stdafx.h"

namespace ravendb::client::http
{
	struct ServerNode
	{
		enum class Role
		{
			NONE,
			PROMOTABLE,
			MEMBER,
			REHAB
		};

		std::string url{};
		std::string database{};
		std::string clusterTag{};
		Role serverRole = Role::NONE;

		ServerNode() = default;

		ServerNode(std::string url, std::string db, std::string tag, Role role = Role::NONE)
			: url(std::move(url))
			, database(std::move(db))
			, clusterTag(std::move(tag))
			, serverRole(role)
		{}

		bool operator==(const ServerNode& other)
		{
			return (&other == this)||(this->url == other.url && this->database == other.database);
		}

	};

	inline void from_json(const nlohmann::json& j, ServerNode& sn)
	{
		sn.url = j.at("Url").get<std::string>();
		sn.database = j.at("Database").get<std::string>();
		sn.clusterTag = j.at("ClusterTag").get<std::string>();
	}
}