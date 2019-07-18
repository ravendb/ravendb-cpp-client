#include "stdafx.h"
#include "IndexStats.h"
#include "json_utils.h"

namespace ravendb::client::documents::indexes
{
	void from_json(const nlohmann::json& j, CollectionStats& cs)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "LastProcessedDocumentEtag", cs.last_processed_document_etag);
		get_val_from_json(j, "LastProcessedTombstoneEtag", cs.last_processed_tombstone_etag);
		get_val_from_json(j, "DocumentLag", cs.document_lag);
		get_val_from_json(j, "TombstoneLag", cs.tombstone_lag);
	}

	void from_json(const nlohmann::json& j, IndexStats& is)
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
