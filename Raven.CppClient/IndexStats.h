#pragma once
#include "DateTimeOffset.h"
#include "IndexState.h"
#include "IndexPriority.h"
#include "IndexLockMode.h"
#include "IndexType.h"
#include "IndexRunningStatus.h"
#include "nlohmann/json.hpp"


namespace ravendb::client::documents::indexes
{
	struct CollectionStats
	{
		int64_t last_processed_document_etag{};
		int64_t last_processed_tombstone_etag{};
		int64_t document_lag = -1;
		int64_t tombstone_lag = -1;;
	};

	void from_json(const nlohmann::json& j, CollectionStats& cs);

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

	void from_json(const nlohmann::json& j, IndexStats& is);
}
