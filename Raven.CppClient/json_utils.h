#pragma once
#include "stdafx.h"

namespace ravendb::client::impl::utils::json_utils
{
	//TODO consider using it everywhere
	//safe if key_name does not exists and more convenient
	template<typename T>
	void get_val_from_json(const nlohmann::json& j, const std::string& key_name, T& field)
	{
		if(auto&& it = j.find(key_name); it != j.end() && (std::strcmp(it->type_name(), "null") != 0))
		{
			field = it->get<T>();
		}
	}

}
