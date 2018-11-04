
#include "stdafx.h"
#include "types.h"
#include "Topology.h"

namespace ravenDB
{
	void ravenDB::from_json(const nlohmann::json& j, ServerNode& p)
	{
		p.url = j.at("Url").get<std::string>();
		p.database = j.at("Database").get<std::string>();
		p.clusterTag = j.at("ClusterTag").get<std::string>();
	}


	void ravenDB::from_json(const nlohmann::json& j, Topology& p)
	{
		p.etag = j.at("Etag").get<int64_t>();
		auto&& nodes = j.at("Nodes").get<nlohmann::json::array_t>();
		p.nodes.clear();
		p.nodes.reserve(nodes.size());

		for (auto& node : nodes) {
			p.nodes.emplace(p.nodes.end(), node);
		}

	}


	void ravenDB::from_json(const nlohmann::json& j, GetDocumentsResult& p)
	{
		p.includes = j.at("Includes").get<nlohmann::json::object_t>();
		p.results = j.at("Results").get<nlohmann::json::array_t>();
		auto next_page_start = j.find("NextPageStart");
		if (next_page_start != j.end())
			p.next_page_start = next_page_start->get<int32_t>();
	}

	void ravenDB::from_json(const nlohmann::json& j, PutResult& p)
	{
		p.id = j.at("Id").get<std::string>();
		p.change_vector = j.at("ChangeVector").get<std::string>();
	}


	//void from_json(const nlohmann::json& j, ravenDB::DatabaseRecord& p)
	//{	
	//}

	void from_json(const nlohmann::json& j, DatabaseRecordWithEtag& p)
	{
		p.etag = j.at("Etag").get<int64_t>();
		p.database_name = j.at("DatabaseName").get<std::string>();
		p.disabled = j.at("Disabled").get<decltype(p.disabled)>();

		//p.data_directory = j.at("DataDirectory").get<decltype(p.database_name)>();
		//p.settings = j.at("Settings").get<decltype(p.settings)>();
		//p.conflict_solver_config = j.at("Settings").get<decltype(p.conflict_solver_config)>();
	}

	void from_json(const nlohmann::json& j, DatabaseTopology& p)
	{
		
	}

	void from_json(const nlohmann::json& j, DatabasePutResult& dpr)
	{
		dpr.raftCommandIndex = j.at("RaftCommandIndex").get<int64_t>();
		dpr.name = j.at("Name").get<std::string>();
		//dpr.topology = j.at("Topology").get<DatabaseTopology>();
		dpr.nodesAddedTo = j.at("NodesAddedTo").get<std::vector<std::string>>();

	}


}
