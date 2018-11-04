#pragma once

namespace ravenDB
{
	struct ServerNode
	{
		enum Role
		{
			NONE,
			PROMOTABLE,
			MEMBER,
			REHAB
		};

		std::string url{};
		std::string database{};
		std::string clusterTag{};
		Role serverRole = NONE;

		ServerNode() = default;
		//ServerNode(const ServerNode& other) = default;
		//ServerNode(ServerNode&& other) = default;
		//ServerNode& operator=(const ServerNode& other) = default;
		//ServerNode& operator=(ServerNode&& other) = default;

		ServerNode(std::string url, std::string db, std::string tag, Role role = NONE)
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

	void from_json(const nlohmann::json& j, ServerNode& p);
}