#pragma once
#include "json_utils.h"

namespace ravendb::client::impl
{
	struct Size
	{
		int64_t size_in_bytes{};
		std::string humane_size{};
	};

	inline void from_json(const nlohmann::json& j, Size& s)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "SizeInBytes", s.size_in_bytes);
		get_val_from_json(j, "HumaneSize", s.humane_size);
	}
}