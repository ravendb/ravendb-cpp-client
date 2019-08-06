#include "stdafx.h"
#include "JsonOperation.h"

namespace ravendb::client::json
{
	std::string JsonOperation::field_path_combine(const std::string& path1, const std::string& path2)
	{
		return path1.empty() ? path2 : path1 + "/" + path2;
	}

	std::string JsonOperation::add_index_field_path(const std::string& field_path, size_t position)
	{
		//return field_path_combine(field_path, std::to_string(position));
		std::ostringstream output{};
		output << field_path << "[" << position << "]";
		return output.str();
	}

	bool JsonOperation::compare_values(const nlohmann::json& old_prop, const nlohmann::json& new_prop)
	{
		if (new_prop.is_number_integer() && old_prop.is_number())
		{
			const auto long_val = new_prop.get<int64_t>();
			const auto double_val = old_prop.get<double>();
			double int_part{};
			return std::modf(double_val, &int_part) == 0.0 && long_val == static_cast<decltype(long_val)>(int_part);
		}
		if (old_prop.is_number_integer() && new_prop.is_number())
		{
			const auto long_val = old_prop.get<int64_t>();
			const auto double_val = new_prop.get<double>();
			double int_part{};
			return std::modf(double_val, &int_part) == 0.0 && long_val == static_cast<decltype(long_val)>(int_part);
		}

		if (old_prop.type() != new_prop.type())
		{
			return false;
		}

		if (old_prop.is_boolean())
		{
			return old_prop.get<bool>() == new_prop.get<bool>();
		}
		if (old_prop.is_string())
		{
			return old_prop.get<std::string>() == new_prop.get<std::string>();
		}
		throw std::invalid_argument("Non value type of arguments");
	}

	bool JsonOperation::compare_json_array(const std::string& field_path, const std::string& id, const nlohmann::json& old_array, const nlohmann::json& new_array, std::optional<std::unordered_map<std::string, std::vector<documents::session::DocumentsChanges>>>& changes_collection, std::optional<std::vector<documents::session::DocumentsChanges>>& doc_changes_collection, const std::string& prop_name)
	{
		if (!old_array.is_array() || !new_array.is_array())
		{
			throw std::invalid_argument("The argument(s) must be of type 'array'");
		}
		if (old_array.size() != new_array.size() && !changes_collection)
		{
			return true;
		}

		size_t position = 0;
		bool is_something_changed = false;

		for (; position < old_array.size() && position < new_array.size(); ++position)
		{
			switch (old_array[position].type())
			{
			case nlohmann::detail::value_t::object:
				if (nlohmann::detail::value_t::object == new_array[position].type())
				{
					is_something_changed |= compare_json(add_index_field_path(field_path, position), id,
						old_array[position], new_array[position], changes_collection, doc_changes_collection);
				}
				else
				{
					is_something_changed = true;
					if (changes_collection)
					{
						new_change(add_index_field_path(field_path, position), prop_name,
							new_array.at(position), old_array.at(position), *doc_changes_collection,
							documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_ADDED);
					}
				}
				break;
			case nlohmann::detail::value_t::array:
				if (nlohmann::detail::value_t::array == new_array[position].type())
				{
					is_something_changed |= compare_json_array(add_index_field_path(field_path, position), id,
						old_array[position], new_array[position],
						changes_collection, doc_changes_collection, prop_name);
				}
				else
				{
					is_something_changed = true;
					if (changes_collection)
					{
						new_change(add_index_field_path(field_path, position), prop_name,
							new_array.at(position), old_array.at(position), *doc_changes_collection,
							documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_CHANGED);
					}
				}
				break;
			case nlohmann::detail::value_t::null:
				if (nlohmann::detail::value_t::null != new_array[position].type())
				{
					is_something_changed = true;
					if (changes_collection)
					{
						new_change(add_index_field_path(field_path, position), prop_name,
							new_array.at(position), old_array.at(position), *doc_changes_collection,
							documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_ADDED);
					}
				}
				break;
			default:
				if (old_array[position] != new_array[position])
				{
					is_something_changed = true;
					if (changes_collection)
					{
						new_change(add_index_field_path(field_path, position), prop_name,
							new_array.at(position), old_array.at(position), *doc_changes_collection,
							documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_CHANGED);
					}
				}
				break;
			}
		}
		if (!changes_collection)
		{
			return is_something_changed;
		}

		for (; position < old_array.size(); ++position)
		{
			new_change(field_path, prop_name, {}, old_array.at(position), *doc_changes_collection,
				documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_REMOVED);
		}
		for (; position < new_array.size(); ++position)
		{
			new_change(field_path, prop_name, new_array.at(position), {}, *doc_changes_collection,
				documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_ADDED);
		}

		return is_something_changed;
	}

	void JsonOperation::new_change(const std::string& field_path, const std::string& name, const nlohmann::json& new_value, const nlohmann::json& old_value, std::vector<documents::session::DocumentsChanges>& doc_changes, documents::session::DocumentsChanges::ChangeType change)
	{
		auto documents_changes = documents::session::DocumentsChanges();
		documents_changes.field_name = name;
		documents_changes.field_new_value = new_value;
		documents_changes.field_old_value = old_value;
		documents_changes.change = change;
		documents_changes.field_path = field_path;
		doc_changes.push_back(std::move(documents_changes));
	}

	bool JsonOperation::entity_changed(const nlohmann::json& new_obj, std::shared_ptr<documents::session::DocumentInfo>& doc_info, std::optional<std::unordered_map<std::string, std::vector<documents::session::DocumentsChanges>>>& changes_collection)
	{
		std::optional<std::vector<documents::session::DocumentsChanges>> doc_changes{};
		if (changes_collection)
		{
			doc_changes.emplace();
		}
		if (!doc_info->new_document && !doc_info->document.is_null())
		{
			return compare_json({}, doc_info->id, doc_info->document, new_obj, changes_collection, doc_changes);
		}
		if (!changes_collection)
		{
			return true;
		}

		new_change({}, {}, {}, {}, *doc_changes, documents::session::DocumentsChanges::ChangeType::DOCUMENT_ADDED);

		changes_collection->insert_or_assign(doc_info->id, std::move(doc_changes).value());
		return true;
	}

	bool JsonOperation::compare_json(const std::string& field_path, const std::string& id,
		const nlohmann::json& original_json, const nlohmann::json& new_json,
		std::optional<std::unordered_map<std::string, std::vector<documents::session::DocumentsChanges>>>& changes_collection,
		std::optional<std::vector<documents::session::DocumentsChanges>>& doc_changes)
	{
		if (!new_json.is_object() || !original_json.is_object())
		{
			throw std::invalid_argument("The argument(s) must be of type 'object'");
		}

		if (changes_collection && !doc_changes)
		{
			doc_changes.emplace();
		}

		std::set<std::string> new_json_props{};
		for (auto& el : new_json.items())
		{
			new_json_props.emplace(el.key());
		}
		std::set<std::string> old_json_props{};
		for (auto& el : original_json.items())
		{
			old_json_props.emplace(el.key());
		}

		std::vector<std::string> new_fields{};
		std::set_difference(new_json_props.begin(), new_json_props.end(), old_json_props.begin(), old_json_props.end(),
			std::inserter(new_fields, new_fields.begin()));

		std::vector<std::string> removed_fields{};
		std::set_difference(old_json_props.begin(), old_json_props.end(), new_json_props.begin(), new_json_props.end(),
			std::inserter(removed_fields, removed_fields.begin()));

		for (const auto& field : removed_fields)
		{
			if (!changes_collection)
			{
				return true;
			}
			new_change(field_path, field, {}, {}, doc_changes.value(), documents::session::DocumentsChanges::ChangeType::REMOVED_FIELD);
		}

		for (const auto& prop : new_json_props)
		{
			if (prop == constants::documents::metadata::LAST_MODIFIED ||
				prop == constants::documents::metadata::COLLECTION ||
				prop == constants::documents::metadata::CHANGE_VECTOR ||
				prop == constants::documents::metadata::ID)
			{
				continue;
			}

			if (std::find(new_fields.cbegin(), new_fields.cend(), prop) != new_fields.cend())
			{
				if (!changes_collection)
				{
					return true;
				}

				new_change(field_path, prop, new_json.at(prop), {}, doc_changes.value(), documents::session::DocumentsChanges::ChangeType::NEW_FIELD);
				continue;
			}

			auto&& new_prop = new_json.at(prop);
			auto&& old_prop = original_json.at(prop);

			bool is_something_changed = false;
			switch (new_prop.type())
			{
			case nlohmann::detail::value_t::number_float:
			case nlohmann::detail::value_t::number_integer:
			case nlohmann::detail::value_t::number_unsigned:
			case nlohmann::detail::value_t::boolean:
			case nlohmann::detail::value_t::string:
				//compare_values() call is a backup , nlohmann::json compare is usually
				//smart enough to compare across number types (like integer 1 with equal float 1.0)
				if (new_prop == old_prop ||
					compare_values(old_prop, new_prop))
				{
					break;
				}
				if (!changes_collection)
				{
					return true;
				}
				new_change(field_path, prop, new_prop, old_prop, doc_changes.value(), documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED);
				break;
			case nlohmann::detail::value_t::null:
				if (old_prop.is_null())
				{
					break;
				}
				if (!changes_collection)
				{
					return true;
				}
				new_change(field_path, prop, {}, old_prop, *doc_changes, documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED);
				break;
			case nlohmann::detail::value_t::array:
				if (!old_prop.is_array())
				{
					if (!changes_collection)
					{
						return true;
					}
					new_change(field_path, prop, new_prop, old_prop, *doc_changes, documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED);
					break;
				}

				is_something_changed = compare_json_array(field_path_combine(field_path, prop), id,
					old_prop, new_prop, changes_collection, doc_changes, prop);
				if (!changes_collection && is_something_changed)
				{
					return true;
				}
				break;
			case nlohmann::detail::value_t::object:
				if (old_prop.is_null())
				{
					if (!changes_collection)
					{
						return true;
					}
					new_change(field_path, prop, new_prop, {}, *doc_changes, documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED);
					break;
				}

				is_something_changed = compare_json(field_path_combine(field_path, prop), id, old_prop, new_prop, changes_collection, doc_changes);
				if (!changes_collection && is_something_changed)
				{
					return true;
				}
				break;
			default:
				throw std::invalid_argument("'new_prop.type()' has unknown value");

			}
		}
		if (!changes_collection || !doc_changes || doc_changes->size() == 0)
		{
			return false;
		}

		changes_collection->insert_or_assign(id, *doc_changes);
		return true;
	}
}