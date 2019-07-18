#pragma once
#include <optional>

namespace ravendb::client::documents::session
{
	struct SessionInfo
	{
		const int32_t session_id;
		std::optional<int64_t> last_cluster_transaction_index{};
		bool no_caching{};

		explicit SessionInfo(int32_t session_id_, std::optional<int64_t> last_cluster_transaction_index_ = {},
			bool no_caching_ = false)
			: session_id(session_id_)
			, last_cluster_transaction_index(last_cluster_transaction_index_)
			, no_caching(no_caching_)
		{}
	};
}
