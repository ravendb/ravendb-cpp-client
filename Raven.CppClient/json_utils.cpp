#include "stdafx.h"
#include "json_utils.h"
#include "utils.h"

namespace ravendb::client::impl::utils::json_utils
{
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

	void create_metadata(nlohmann::json& j,
		const std::string& full_class_name,
		const std::optional<std::string>& collection_name,
		const std::optional<std::string>& id_field)
	{
		if(!j.is_object())
		{
			throw std::invalid_argument("j must be an object");
		}

		auto& metadata = j[constants::documents::metadata::KEY];

		//setting the class name
		if(is_blank(full_class_name))
		{
			throw std::invalid_argument("full_class_name can't be blank");
		}
		
		set_val_to_json(metadata, constants::documents::metadata::RAVEN_C_PLUS_PLUS_TYPE, full_class_name);

		//setting the collection name
		if (collection_name)
		{
			if (is_blank(*collection_name))
			{
				throw std::invalid_argument("collection_name, if specified, can't be blank");
			}
			set_val_to_json(metadata, constants::documents::metadata::COLLECTION, *collection_name);
		}else
		{
			set_val_to_json(metadata, constants::documents::metadata::COLLECTION, nullptr);
		}

		//setting the id field

		if(id_field)
		{
			if (auto it = j.find(*id_field);
				it == j.end())
			{
				throw std::invalid_argument("id_field is specified, but absent in j");
			}else
			{
				if(!it->is_string())
				{
					throw std::invalid_argument("id_field if specified, must point to a string field");
				}
				set_val_to_json(metadata, constants::documents::metadata::ID_PROPERTY, *it);
			}
		}
		//id_field was not specified
		set_val_to_json(metadata, constants::documents::metadata::ID_PROPERTY, nullptr);

		set_val_to_json(metadata, constants::documents::metadata::ID, nullptr);
	}
}
