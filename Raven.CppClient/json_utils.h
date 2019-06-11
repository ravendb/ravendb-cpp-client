#pragma once
#include <optional>
#include "MillisToTimespanConverter.h"
#include "json.hpp"
#include "Constants.h"

namespace ravendb::client::impl::utils::json_utils
{
	//safe if key_name does not exists and more convenient
	template<typename T>
	bool get_val_from_json(const nlohmann::json& j, const std::string& key_name, T& field)
	{
		if(auto&& it = j.find(key_name); it != j.end() && ! it->is_null())
		{
			it->get_to<T>(field);
			return true;
		}
		return false;
	}

	template<>
	bool get_val_from_json(const nlohmann::json& j, const std::string& key_name, nlohmann::json& field);

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

	//TODO consider using universal references instead of/in addition to const&

	template<typename T>
	bool set_val_to_json(nlohmann::json& j, const std::string& key_name, const T& field)
	{
		bool res = false;
		if (auto it = j.find(key_name);
			it == j.end())
		{
			std::tie(std::ignore, res) = j.emplace(key_name, field);
			return res;
		}
		else
		{
			it.value() = field;
		}
		return true;
	}

	template<typename T>
	bool set_val_to_json(nlohmann::json& j, const std::string& key_name, const std::optional<T>& field,
		bool set_null_if_no_value = true)
	{
		if(field.has_value())
		{
			return set_val_to_json(j, key_name, field.value());
		}
		if(set_null_if_no_value)
		{
			return set_val_to_json(j, key_name, nullptr);
		}
		return false;
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

	template<>
	bool set_val_to_json(nlohmann::json& j, const std::string& key_name, const std::chrono::milliseconds& field);

}

