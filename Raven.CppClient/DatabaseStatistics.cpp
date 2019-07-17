#include "stdafx.h"
#include "DatabaseStatistics.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	std::vector<IndexInformation> DatabaseStatistics::get_stale_indexes() const
	{
		auto stale_indexes = std::vector<IndexInformation>{};
		std::copy_if(indexes.cbegin(), indexes.cend(), std::back_inserter(stale_indexes),
			[](const IndexInformation& index)->bool
		{
			return index.is_stale;
		});
		return stale_indexes;
	}

	void from_json(const nlohmann::json& j, DatabaseStatistics& dbs)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "LastDocEtag", dbs.last_doc_etag);
		get_val_from_json(j, "CountOfIndexes", dbs.count_of_indexes);
		get_val_from_json(j, "CountOfDocuments", dbs.count_of_documents);
		get_val_from_json(j, "CountOfRevisionDocuments", dbs.count_of_revision_documents);
		get_val_from_json(j, "CountOfDocumentsConflicts", dbs.count_of_documents_conflicts);
		get_val_from_json(j, "CountOfTombstones", dbs.count_of_tombstones);
		get_val_from_json(j, "CountOfConflicts", dbs.count_of_conflicts);
		get_val_from_json(j, "CountOfAttachments", dbs.count_of_attachments);
		get_val_from_json(j, "CountOfCounters", dbs.count_of_counters);
		get_val_from_json(j, "CountOfUniqueAttachments", dbs.count_of_unique_attachments);

		get_val_from_json(j, "Indexes", dbs.indexes);

		get_val_from_json(j, "DatabaseChangeVector", dbs.database_change_vector);
		get_val_from_json(j, "DatabaseId", dbs.database_id);
		get_val_from_json(j, "Is64Bit", dbs.is_64_bit);
		get_val_from_json(j, "Pager", dbs.pager);
		get_val_from_json(j, "LastIndexingTime", dbs.last_indexing_time);
		get_val_from_json(j, "SizeOnDisk", dbs.size_on_disk);
		get_val_from_json(j, "TempBuffersSizeOnDisk", dbs.temp_buffers_size_on_disk);
		get_val_from_json(j, "NumberOfTransactionMergerQueueOperations",
		                  dbs.number_of_transaction_merger_queue_operations);
	}
}
