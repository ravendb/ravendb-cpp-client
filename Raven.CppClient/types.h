#pragma once

#include "stdafx.h"
#include "ServerNode.h"

namespace raven 
{
	struct GetDocumentsResult 
	{
		nlohmann::json::object_t includes;
		nlohmann::json::array_t results;
		int32_t next_page_start = 0;
	};
	
	void from_json(const nlohmann::json& j, GetDocumentsResult& p);


	struct PutResult
	{
		std::string id{};
		std::string change_vector{};
	};

	void from_json(const nlohmann::json& j, PutResult& p);

	struct ScriptResolver
	{
		std::string script{};
		std::chrono::system_clock::time_point last_modified_time{};//should be utc_clock(c++20)
	};

	struct ConflictSolver
	{
		std::map<std::string, ScriptResolver> resolve_by_collection{};
		bool resolve_to_latest = true;//am I right ?
	};


	struct DatabaseRecord
	{
		std::string database_name{};
		bool disabled = false;
		std::string data_directory{};
		std::unordered_map<std::string, std::string> settings{};
		ConflictSolver conflict_solver_config{};
	};

	//void from_json(const nlohmann::json& j, DatabaseRecord& p);

	struct DatabaseRecordWithEtag : public DatabaseRecord
	{
		int64_t etag = -1;
	};

	void from_json(const nlohmann::json& j, DatabaseRecordWithEtag& p);


	enum class DatabasePromotionStatus
	{
		WAITING_FOR_FIRST_PROMOTION,
		NOT_RESPONDING,
		INDEX_NOT_UP_TO_DATE,
		CHANGE_VECTOR_NOT_MERGED,
		WAITING_FOR_RESPONSE,
		OK
	};

	struct LeaderStamp
	{
		int64_t index = -1;
		int64_t term = -1;
		int64_t leadersTicks = -1;
	};


	struct DatabaseTopology
	{
		std::vector<std::string> members{};
		std::vector<std::string> promotables{};
		std::vector<std::string> rehabs{};

		std::unordered_map<std::string, std::string> predefinedMentors{};
		std::unordered_map<std::string, std::string> demotionReasons{};
		std::unordered_map<std::string, DatabasePromotionStatus> promotablesStatus{};
		int32_t replicationFactor = 1;
		bool dynamicNodesDistribution = true;
		LeaderStamp stamp{};
	};

	void from_json(const nlohmann::json& j, DatabaseTopology& p);

	struct DatabasePutResult
	{
		int64_t raftCommandIndex{};
		std::string name{};
		DatabaseTopology topology{};
		std::vector<std::string> nodesAddedTo{};
	};

	
	void from_json(const nlohmann::json& j, DatabasePutResult& p);



}