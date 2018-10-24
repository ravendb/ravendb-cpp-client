#pragma once

namespace raven
{
	struct Topology
	{
		std::vector<ServerNode> nodes;
		int64_t etag = -1;//TODO : set default value ! 

		//Topology() = default;
		//Topology(const Topology& other) = default;
		//Topology(Topology&& other) = default;
		//Topology& operator=(const Topology& other) = default;
		//Topology& operator=(Topology&& other) = default;
	};

	void from_json(const nlohmann::json& j, Topology& p);
}