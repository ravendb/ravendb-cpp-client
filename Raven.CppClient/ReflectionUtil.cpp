#include "stdafx.h"
#include "ReflectionUtil.h"
#include "DocumentConventions.h"
#include "Constants.h"

namespace ravendb::client::impl
{
	std::unordered_map<std::type_index, std::string> ReflectionUtil::full_name_cache{};


	const std::string& ReflectionUtil::get_full_name_without_version(std::type_index entity_type)
	{
		if(auto it = full_name_cache.find(entity_type);
			it != full_name_cache.end())
		{
			return it->second;
		}

		if (auto full_name = documents::conventions::DocumentConventions::get_serialized_type(entity_type);
			!full_name)
		{
			throw std::invalid_argument("The type 'entity_type' is not registered at DocumentConventions."
				" Please register in DocumentConventions first.");
		}
		else if(auto metadata_it = full_name->find(constants::documents::metadata::KEY);
			metadata_it == full_name->end())
		{
				throw std::invalid_argument("The type 'entity_type' must contain metadata");
		}
		else if(auto cpp_type_name_it = metadata_it->find(constants::documents::metadata::RAVEN_C_PLUS_PLUS_TYPE);
			cpp_type_name_it == metadata_it->end())
		{
			throw std::invalid_argument("The type 'entity_type' must contain RAVEN_C_PLUS_PLUS_TYPE"
				" in it's metadata");
		}else
		{
			full_name_cache.insert_or_assign(entity_type, *cpp_type_name_it);
			return full_name_cache[entity_type];
		}
	}
}
