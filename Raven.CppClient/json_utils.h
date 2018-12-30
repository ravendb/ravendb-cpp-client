#pragma once
#include "stdafx.h"
#include <iomanip>
#include "utils.h"

namespace ravendb::client::impl::utils::json_utils
{
	//safe if key_name does not exists and more convenient
	template<typename T>
	bool get_val_from_json(const nlohmann::json& j, const std::string& key_name, T& field)
	{
		if(auto&& it = j.find(key_name); it != j.end() && ! it->is_null())
		{
			field = it->get<T>();
			return true;
		}
		return false;
	}

	template<typename T>
	bool get_val_from_json(const nlohmann::json& j, const std::string& key_name, std::optional<T>& field)
	{
		T field_val;
		if(get_val_from_json(j, key_name, field_val))
		{
			field = std::move(field_val);
			return true;
		}else
		{
			field.reset();
			return false;
		}
	}

	template<typename T>
	bool get_val_from_json(const nlohmann::json& j, std::vector<T>& array)
	{
		if (j.is_array())
		{
			array.clear();
			array.reserve(j.size());

			for (const auto& val : j)
			{
				array.push_back(val.get<T>());
			}
			return true;
		}
		return false;
	}
	
	template<typename T>
	bool get_val_from_json(const nlohmann::json& j, const std::string& key_name, std::vector<T>& array)
	{
		if (auto&& it = j.find(key_name); it != j.end())
		{
			return get_val_from_json(*it, array);
		}
		return false;
	}

	template<typename T>
	bool set_val_to_json(nlohmann::json& j, const std::string& key_name, const T& field)
	{
		bool res = false;
		std::tie(std::ignore, res) = j.emplace(key_name, field);
		return res;
	}

	template<typename T>
	bool set_val_to_json(nlohmann::json& j, const std::string& key_name, const std::optional<T>& field)
	{
		if(field.has_value())
		{
			return set_val_to_json(j, key_name, field.value());
		}else
		{
			return false;
		}
	}

	template<typename T>
	bool set_val_to_json(nlohmann::json& j, const std::vector<T>& array)
	{
		j.clear();
		for(const auto& val : array)
		{
			j.push_back(val);
		}
		return true;
	}

	template<typename T>
	bool set_val_to_json(nlohmann::json& j, const std::string& key_name, const std::vector<T>& array)
	{
		nlohmann::json temp{};
		set_val_to_json(temp, array);

		bool res = false;
		std::tie(std::ignore, res) = j.emplace(key_name, std::move(temp));
		return res;
	}
}




namespace std::chrono
{
	//serialization in C# TimeSpan format : d.hh:mm:ss.sssssss or hh:mm:ss.sssssss
	inline void to_json(nlohmann::json& j, const std::chrono::milliseconds& msec)
	{
		j = ravendb::client::impl::utils::millis_to_timespan(msec);
	}
}
