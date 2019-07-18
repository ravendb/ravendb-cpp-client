#include "stdafx.h"
#include "IndexDefinition.h"
#include "utils.h"
#include "json_utils.h"
#include <regex>

namespace ravendb::client::documents::indexes
{
	IndexType IndexDefinition::detect_static_index_type() const
	{
		if(maps.empty())
		{
			throw std::invalid_argument("Index definitions contains no Maps");
		}

		const auto& first_map = *maps.cbegin();

		static const std::regex elim_reg("(?:/\\*(?:[^*]|(?:\\*+[^*/]))*\\*+/)|(?://.*)");

		auto fixed_first_map = std::regex_replace(first_map, elim_reg, "");
		impl::utils::left_trim(fixed_first_map);
		impl::utils::right_trim(fixed_first_map);

		if(fixed_first_map.size() > 4 && (
			"from" == fixed_first_map.substr(0,4) ||
			"docs" == fixed_first_map.substr(0, 4)))
			// C# indexes must start with "from" for query syntax or
			// "docs" for method syntax
		{
			if(!reduce || impl::utils::is_blank(*reduce))
			{
				return IndexType::MAP;
			}
			return IndexType::MAP_REDUCE;
		}
		if (!reduce || impl::utils::is_blank(*reduce))
		{
			return IndexType::JAVASCRIPT_MAP;
		}
		return IndexType::JAVASCRIPT_MAP_REDUCE;
	}

	IndexDefinition::operator std::string() const
	{
		return name;
	}

	IndexType IndexDefinition::get_type()
	{
		if(!index_type || index_type == IndexType::NONE)
		{
			index_type.emplace(detect_static_index_type());
		}
		return *index_type;
	}

	bool operator==(const IndexDefinition& lhs, const IndexDefinition& rhs)
	{
		return lhs.name == rhs.name
			&& lhs.priority == rhs.priority
			&& lhs.lock_mode == rhs.lock_mode
			&& (lhs.additional_sources ?
				lhs.additional_sources == rhs.additional_sources : !rhs.additional_sources || rhs.additional_sources->empty())
			&& lhs.maps == rhs.maps
			&& lhs.reduce == rhs.reduce
			&& lhs.fields == rhs.fields
			&& lhs.configuration == rhs.configuration
			&& lhs.index_type == rhs.index_type
			&& lhs.output_reduce_to_collection == rhs.output_reduce_to_collection;
	}

	void to_json(nlohmann::json& j, const IndexDefinition& id)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", id.name);
		set_val_to_json(j, "Priority", id.priority);
		set_val_to_json(j, "LockMode", id.lock_mode);
		set_val_to_json(j, "AdditionalSources", id.additional_sources);
		set_val_to_json(j, "Maps", id.maps);
		set_val_to_json(j, "Reduce", id.reduce);
		set_val_to_json(j, "Fields", id.fields);
		set_val_to_json(j, "Configuration", id.configuration);
		set_val_to_json(j, "Type", id.index_type);
		set_val_to_json(j, "OutputReduceToCollection", id.output_reduce_to_collection);
	}

	void from_json(const nlohmann::json& j, IndexDefinition& id)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", id.name);
		get_val_from_json(j, "Priority", id.priority);
		get_val_from_json(j, "LockMode", id.lock_mode);
		get_val_from_json(j, "AdditionalSources", id.additional_sources);
		get_val_from_json(j, "Maps", id.maps);
		get_val_from_json(j, "Reduce", id.reduce);
		get_val_from_json(j, "Fields", id.fields);
		get_val_from_json(j, "Configuration", id.configuration);
		get_val_from_json(j, "Type", id.index_type);
		get_val_from_json(j, "OutputReduceToCollection", id.output_reduce_to_collection);
	}
}
