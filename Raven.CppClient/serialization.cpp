
#include "stdafx.h"
#include "types.h"

namespace ravendb::client
{


	void ravendb::client::from_json(const nlohmann::json& j, GetDocumentsResult& p)
	{
		p.includes = j.at("Includes").get<nlohmann::json::object_t>();
		p.results = j.at("Results").get<nlohmann::json::array_t>();
		auto next_page_start = j.find("NextPageStart");
		if (next_page_start != j.end())
			p.next_page_start = next_page_start->get<int32_t>();
	}

	void ravendb::client::from_json(const nlohmann::json& j, PutResult& p)
	{
		p.id = j.at("Id").get<std::string>();
		p.change_vector = j.at("ChangeVector").get<std::string>();
	}


	//void from_json(const nlohmann::json& j, ravendb::client::DatabaseRecord& p)
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

	//void from_json(const nlohmann::json& j, DatabaseTopology& p)
	//{
	//	
	//}

	void from_json(const nlohmann::json& j, DatabasePutResult& dpr)
	{
		dpr.raft_command_index = j.at("RaftCommandIndex").get<int64_t>();
		dpr.name = j.at("Name").get<std::string>();
		//dpr.topology = j.at("Topology").get<DatabaseTopology>();
		dpr.nodesAddedTo = j.at("NodesAddedTo").get<std::vector<std::string>>();
	}

	void from_json(const nlohmann::json& j, DeleteDatabaseResult& ddr)
	{
		ddr.raft_command_index = j.at("RaftCommandIndex").get<int64_t>();
		ddr.pending_deletes = j.at("PendingDeletes").get<std::vector<std::string>>();
	}

}
