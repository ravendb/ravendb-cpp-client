#pragma once
#include "json_utils.h"
#include "DateTimeOffset.h"
#include "IndexState.h"
#include "IndexPriority.h"
#include "IndexLockMode.h"
#include "IndexType.h"
#include "IndexRunningStatus.h"

namespace ravendb::client::documents::indexes
{
	struct CollectionStats
	{
		int64_t last_processed_document_etag{};
		int64_t last_processed_tombstone_etag{};
		int64_t document_lag = -1;
		int64_t tombstone_lag = -1;;
	};

	inline void from_json(const nlohmann::json& j, CollectionStats& cs)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "LastProcessedDocumentEtag", cs.last_processed_document_etag);
		get_val_from_json(j, "LastProcessedTombstoneEtag", cs.last_processed_tombstone_etag);
		get_val_from_json(j, "DocumentLag", cs.document_lag);
		get_val_from_json(j, "TombstoneLag", cs.tombstone_lag);
	}

	struct IndexStats
	{
		std::string name{};

		int32_t map_attempts{};
		int32_t map_successes{};
		int32_t map_errors{};

		std::optional<int32_t> reduce_attempts{};
		std::optional<int32_t> reduce_successes{};
		std::optional<int32_t> reduce_errors{};

		double mapped_per_second_rate{};
		double reduced_per_second_rate{};
		int32_t max_number_of_outputs_per_document{};

		std::unordered_map<std::string, CollectionStats> collections{};
		std::optional<impl::DateTimeOffset> last_querying_time{};

		IndexState state = IndexState::UNSET;
		IndexPriority priority = IndexPriority::UNSET;

		impl::DateTimeOffset created_timestamp{};
		std::optional<impl::DateTimeOffset> last_indexing_time{};

		bool is_stale = false;
		IndexLockMode lock_mode = IndexLockMode::UNSET;
		IndexType type = IndexType::UNSET;
		IndexRunningStatus status = IndexRunningStatus::UNSET;

		int32_t entries_count{};
		int32_t errors_count{};

		bool is_test_index = false;
	};

	inline void from_json(const nlohmann::json& j, IndexStats& is)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", is.name);

		get_val_from_json(j, "MapAttempts", is.map_attempts);
		get_val_from_json(j, "MapSuccesses", is.map_successes);
		get_val_from_json(j, "MapErrors", is.map_errors);

		get_val_from_json(j, "ReduceAttempts", is.reduce_attempts);
		get_val_from_json(j, "ReduceSuccesses", is.reduce_successes);
		get_val_from_json(j, "ReduceErrors", is.reduce_errors);

		get_val_from_json(j, "MappedPerSecondRate", is.mapped_per_second_rate);
		get_val_from_json(j, "ReducedPerSecondRate", is.reduced_per_second_rate);
		get_val_from_json(j, "MaxNumberOfOutputsPerDocument", is.max_number_of_outputs_per_document);

		get_val_from_json(j, "Collections", is.collections);
		get_val_from_json(j, "LastQueryingTime", is.last_querying_time);

		get_val_from_json(j, "State", is.state);
		get_val_from_json(j, "Priority", is.priority);

		get_val_from_json(j, "CreatedTimestamp", is.created_timestamp);
		get_val_from_json(j, "LastIndexingTime", is.last_indexing_time);

		get_val_from_json(j, "IsStale", is.is_stale);
		get_val_from_json(j, "LockMode", is.lock_mode);
		get_val_from_json(j, "Type", is.type);
		get_val_from_json(j, "Status", is.status);

		get_val_from_json(j, "EntriesCount", is.entries_count);
		get_val_from_json(j, "ErrorsCount", is.errors_count);

		get_val_from_json(j, "IsTestIndex", is.is_test_index);		
	}
}
