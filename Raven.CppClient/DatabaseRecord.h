#pragma once
#include "ConflictSolver.h"
#include "DatabaseTopology.h"
#include "IndexDefinition.h"
#include "ClientConfiguration.h"

namespace  ravendb::client::serverwide
{
	struct DatabaseRecord
	{
		//TODO add missing fields
		std::string database_name{};
		bool disabled{ false };
		bool encrypted{};
		int64_t etag_for_backup{};
		//TODO Map<String, DeletionInProgressStatus> deletionInProgress;
		DatabaseTopology topology{};
		ConflictSolver conflict_solver_config{};
		//TODO Map<String, SorterDefinition> sorters = new HashMap<>();
		std::unordered_map<std::string, documents::indexes::IndexDefinition> indexes{};
		//TODO Map<String, AutoIndexDefinition> autoIndexes;
		std::unordered_map<std::string, std::string> settings{};
		//TODO RevisionsConfiguration revisions;
		//TODO ExpirationConfiguration expiration;
		//TODO List<PeriodicBackupConfiguration> periodicBackups = new ArrayList<>();
		//TODO List<ExternalReplication> externalReplications = new ArrayList<>();
		//TODO List<PullReplicationAsSink> sinkPullReplications = new ArrayList<>();
		//TODO List<PullReplicationDefinition> hubPullReplications = new ArrayList<>();
		//TODO Map<String, RavenConnectionString> ravenConnectionStrings = new HashMap<>();
		//TODO Map<String, SqlConnectionString> sqlConnectionStrings = new HashMap<>();
		//TODO List<RavenEtlConfiguration> ravenEtls = new ArrayList<>();
		//TODO List<SqlEtlConfiguration> sqlEtls = new ArrayList<>();
		std::optional<documents::operations::configuration::ClientConfiguration> client{};
		//TODO StudioConfiguration studio;
		int64_t truncated_cluster_transaction_commands_count{};
		
		std::string data_directory{};
		std::optional<int64_t> etag{};

		DatabaseRecord() = default;
		explicit DatabaseRecord(std::string database_name_param);
	};

	void to_json(nlohmann::json& j, const DatabaseRecord& dbr);

	void from_json(const nlohmann::json& j, DatabaseRecord& dbr);
}
