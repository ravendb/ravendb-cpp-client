#pragma once

namespace ravendb::client::serverwide::commands
{
	struct TcpConnectionInfo
	{
		int32_t port{};
		std::string url{};
		std::optional<std::string> certificate{};
	};

	void to_json(nlohmann::json& j, const TcpConnectionInfo& tci);

	void from_json(const nlohmann::json& j, TcpConnectionInfo& tci);
}
