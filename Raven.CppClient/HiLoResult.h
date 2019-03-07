#pragma once
#include "DateTimeOffset.h"
#include "json_utils.h"

namespace ravendb::client::documents::identity
{
	struct HiLoResult
	{
		std::string prefix{};
		int64_t low{};
		int64_t high{};
		int64_t last_size{};
		std::string server_tag{};
		impl::DateTimeOffset last_range_at{};
	};

	inline void from_json(const nlohmann::json& j, HiLoResult& hlr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Prefix", hlr.prefix);
		get_val_from_json(j, "Low", hlr.low);
		get_val_from_json(j, "High", hlr.high);
		get_val_from_json(j, "LastSize", hlr.last_size);
		get_val_from_json(j, "ServerTag", hlr.server_tag);
		get_val_from_json(j, "LastRangeAt", hlr.last_range_at);
	}
}
