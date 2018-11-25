#pragma once
#include "stdafx.h"

namespace ravendb::client::impl::utils::json_utils
{
	//TODO consider using it everywhere
	//safe if key_name does not exists and more convenient
	template<typename T>
	bool get_val_from_json(const nlohmann::json& j, const std::string& key_name, T& field)
	{
		if(auto&& it = j.find(key_name); it != j.end() && (std::strcmp(it->type_name(), "null") != 0))
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

	//TODO consider using : 
	//void set_val_to_json(nlohmann::json& j, const std::string& key_name, const T& field)
	//void set_val_to_json(nlohmann::json& j, const std::string& key_name, const std::optional<T>& field))
}
