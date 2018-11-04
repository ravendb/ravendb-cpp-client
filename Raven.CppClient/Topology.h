#pragma once

namespace ravenDB
{
	struct Topology
	{
		std::vector<ServerNode> nodes;
		int64_t etag = -1;

		//Topology() = default;
		//Topology(const Topology& other) = default;
		//Topology(Topology&& other) = default;
		//Topology& operator=(const Topology& other) = default;
		//Topology& operator=(Topology&& other) = default;
	};

	void from_json(const nlohmann::json& j, Topology& p);
}