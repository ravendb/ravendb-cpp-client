#pragma once
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

	void from_json(const nlohmann::json& j, DatabaseStatistics& dbs);
}
