#pragma once
#include "nlohmann/json.hpp"

namespace ravendb::client::http
{
	enum class Role
	{
		NONE,
		PROMOTABLE,
		MEMBER,
		REHAB,
		INVALID_VALUE = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(Role,
		{
			{Role::INVALID_VALUE, nullptr},
			{Role::NONE, "None"},
			{Role::PROMOTABLE , "Promotable"},
			{Role::MEMBER, "Member"},
			{Role::REHAB, "Rehab"}
		});

	struct ServerNode
	{
		std::string url{};
		std::string database{};
		std::string cluster_tag{};
		Role server_role = Role::NONE;

		ServerNode() = default;

		ServerNode(std::string url, std::string db, std::string tag, Role role = Role::NONE);

		friend bool operator==(const ServerNode& lhs, const ServerNode& rhs);

		friend bool operator!=(const ServerNode& lhs, const ServerNode& rhs);
	};

	void from_json(const nlohmann::json& j, ServerNode& sn);

	void to_json(nlohmann::json& j, const ServerNode& sn);

	struct CompareSharedPtrConstServerNode
	{
		bool operator()(std::shared_ptr<const ServerNode> lhs, std::shared_ptr<const ServerNode> rhs) const;
	};
}

namespace std
{
	template<>
	struct hash<ravendb::client::http::ServerNode>
	{
		typedef ravendb::client::http::ServerNode argument_type;
		typedef std::size_t result_type;
		//hash calculation as in the Java client
		result_type operator()(argument_type const& a) const noexcept
		{
			const auto
				val1 = std::hash<std::string>{}(a.url),
				val2 = std::hash<std::string>{}(a.database);
			return val1 * 31 + val2;
		}
	};

	template<>
	struct hash<std::shared_ptr<ravendb::client::http::ServerNode>>
	{
		typedef std::shared_ptr<ravendb::client::http::ServerNode> argument_type;
		typedef std::size_t result_type;
		//hash calculation as in the Java client
		result_type operator()(argument_type const& a) const noexcept
		{
			return a ? std::hash<ravendb::client::http::ServerNode>{}(*a) : 0;
		}
	};

	template<>
	struct hash<std::shared_ptr<const ravendb::client::http::ServerNode>>
	{
		typedef std::shared_ptr<const ravendb::client::http::ServerNode> argument_type;
		typedef std::size_t result_type;
		//hash calculation as in the Java client
		result_type operator()(argument_type const& a) const noexcept
		{
			return std::hash<std::shared_ptr<ravendb::client::http::ServerNode>>{}(
				std::const_pointer_cast<ravendb::client::http::ServerNode>(
					const_cast<std::shared_ptr<const ravendb::client::http::ServerNode>&>(a)));
		}
	};
}