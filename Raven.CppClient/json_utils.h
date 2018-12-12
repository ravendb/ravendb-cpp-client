#pragma once
#include "stdafx.h"
#include <iomanip>

namespace ravendb::client::impl::utils::json_utils
{
	//TODO add std::vector overloads

	//TODO consider using it everywhere
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
}

namespace std::chrono
{
	//serialization in C# TimeSpan format : d.hh:mm:ss.sssssss or hh:mm:ss.sssssss
	inline void to_json(nlohmann::json& j, const std::chrono::milliseconds& msec)
	{
		std::ostringstream time_dur;
		auto h = duration_cast<hours>(msec).count();
		auto m = duration_cast<minutes>(msec%hours(1)).count();
		auto s = duration_cast<seconds>(msec%minutes(1)).count();
		auto ms = (msec%seconds(1)).count();

		//num of days and hours remained
		auto d = h / 24;
		h %= 24;

		if (d > 0)
		{
			time_dur << d << ".";
		}

		time_dur << std::setfill('0') <<
			std::setw(2) << h << ":" <<
			std::setw(2) << m << ":" <<
			std::setw(2) << s << "." <<
			std::setw(7) << ms;

		j = time_dur.str();
	}
}
