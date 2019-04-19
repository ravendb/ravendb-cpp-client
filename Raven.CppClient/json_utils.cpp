#include "stdafx.h"
#include "json_utils.h"
#include "utils.h"

namespace ravendb::client::impl::utils::json_utils
{
	template <>
	bool get_val_from_json<nlohmann::basic_json<>>(const nlohmann::json& j, const std::string& key_name,
		nlohmann::json& field)
	{
		if (auto&& it = j.find(key_name); it != j.end() && !it->is_null())
		{
			field = *it;
			return true;
		}
		return false;
	}

	template <>
	bool set_val_to_json<std::chrono::milliseconds>(nlohmann::json& j, const std::string& key_name, const std::chrono::milliseconds& field)
	{
		bool res = false;
		if (auto it = j.find(key_name);
			it == j.end())
		{
			std::tie(std::ignore, res) = j.emplace(key_name, MillisToTimeSpanConverter(field));
		}
		else
		{
			it.value() = MillisToTimeSpanConverter(field);
			res = true;
		}
		return res;
	}
}
