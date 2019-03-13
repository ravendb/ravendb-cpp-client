#pragma once
#include "json_utils.h"
#include "DateTimeOffset.h"
#include "IndexInformation.h"
#include "Size.h"

namespace ravendb::client::documents::operations
{
	struct DatabaseStatistics
	{
		std::optional<int64_t> last_doc_etag{};
		int32_t count_of_indexes{};
		int64_t count_of_documents{};
		int64_t count_of_revision_documents{};
		int64_t count_of_documents_conflicts{};
		int64_t count_of_tombstones{};
		int64_t count_of_conflicts{};
		int64_t count_of_attachments{};
		int64_t count_of_counters{};
		int64_t count_of_unique_attachments{};

		std::vector<IndexInformation> indexes{};

		std::string database_change_vector{};
		std::string database_id{};
		bool is_64_bit{};
		std::string pager{};
		std::optional<impl::DateTimeOffset> last_indexing_time{};
		impl::Size size_on_disk{};
		impl::Size temp_buffers_size_on_disk{};
		int32_t number_of_transaction_merger_queue_operations{};

		std::vector<IndexInformation> get_stale_indexes() const;
	};

	inline std::vector<IndexInformation> DatabaseStatistics::get_stale_indexes() const
	{
		auto stale_indexes = std::vector<IndexInformation>{};
		std::copy_if(indexes.cbegin(), indexes.cend(), std::back_inserter(stale_indexes),
			[](const IndexInformation& index)->bool
		{
			return index.is_stale;
		});
		return stale_indexes;
	}

	inline void from_json(const nlohmann::json& j, DatabaseStatistics& dbs)
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
		get_val_from_json(j, "NumberOfTransactionMergerQueueOperations", dbs.number_of_transaction_merger_queue_operations);
	}
}
