#pragma once
#include "DateTimeOffset.h"
#include "json.hpp"

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

	void from_json(const nlohmann::json& j, HiLoResult& hlr);

}
