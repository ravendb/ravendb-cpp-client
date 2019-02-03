#include "stdafx.h"
#include "EntityToJson.h"
#include "DocumentInfo.h"
#include "InMemoryDocumentSessionOperations.h"
#include "json_utils.h"
#include "MetadataAsDictionary.h"

namespace ravendb::client::documents::session
{
	EntityToJson::EntityToJson(InMemoryDocumentSessionOperations& session)
		: _session(session)
	{}

	void EntityToJson::write_metadata(nlohmann::json& json, std::shared_ptr<DocumentInfo> doc_info)
	{
		if (!doc_info)
		{
			return;
		}
		bool set_metadata = false;
		nlohmann::json metadata_json = nlohmann::json::object();

		if (!doc_info->metadata.empty())
		{
			set_metadata = true;
			metadata_json = doc_info->metadata;
		}
		else if (doc_info->metadata_instance)
		{
			set_metadata = true;
			for (auto& metadata_it : doc_info->metadata_instance->get_dictionary())
			{
				impl::utils::json_utils::set_val_to_json(metadata_json, metadata_it.first,
					basic_metadata_to_json_converter(metadata_it.second));
			}
		}
		if (!doc_info->collection.empty())
		{
			set_metadata = true;
			impl::utils::json_utils::set_val_to_json(metadata_json,
				constants::documents::metadata::COLLECTION, doc_info->collection);
		}

		if (set_metadata)
		{
			impl::utils::json_utils::set_val_to_json(json, constants::documents::metadata::KEY,
				metadata_json);
		}
	}

	nlohmann::json EntityToJson::convert_entity_to_json(std::shared_ptr<void> entity, std::shared_ptr<DocumentInfo> doc_info,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		if (!doc_info || !doc_info->entity && !entity)
		{
			throw std::invalid_argument("doc_info should not be empty "
				"and at least entity or doc_info->entity should not be empty");
		}
		if (!entity)
		{
			entity = doc_info->entity;
		}
		if ((!to_json || !to_json.value()) && !doc_info->to_json_converter)
		{
			throw std::runtime_error("for document id = " + doc_info->id +
				" to_json_converter is missing in the session.");
		}
		auto json = (to_json && to_json.value()) ? to_json.value()(entity) : doc_info->to_json_converter(entity);

		write_metadata(json, doc_info);

		return json;
	}

	nlohmann::json EntityToJson::basic_metadata_to_json_converter(const std::any& object)
	{
		//TODO think of more types or other solution
		if (!object.has_value())
		{
			return nullptr;
		}
		if (auto val_ptr = std::any_cast<std::string>(&object);
			nullptr != val_ptr)
		{
			return *val_ptr;
		}
		if (auto val_ptr = std::any_cast<const char*>(&object);
			nullptr != val_ptr)
		{
			return std::string(*val_ptr);
		}
		if (auto val_ptr = std::any_cast<int64_t>(&object);
			nullptr != val_ptr)
		{
			return *val_ptr;
		}
		if (auto val_ptr = std::any_cast<uint64_t>(&object);
			nullptr != val_ptr)
		{
			return *val_ptr;
		}
		if (auto val_ptr = std::any_cast<double>(&object);
			nullptr != val_ptr)
		{
			return *val_ptr;
		}
		if (auto val_ptr = std::any_cast<bool>(&object);
			nullptr != val_ptr)
		{
			return *val_ptr;
		}
		if (auto val_ptr = std::any_cast<std::shared_ptr<json::MetadataAsDictionary>>(&object);
			nullptr != val_ptr)
		{
			const auto& map = val_ptr->get()->get_dictionary();
			nlohmann::json j = nlohmann::json::object();

			for (const auto&[key, val] : map)
			{
				impl::utils::json_utils::set_val_to_json(j, key, basic_metadata_to_json_converter(val));
			}
			return j;
		}
		if (auto val_ptr = std::any_cast<std::vector<std::any>>(&object);
			nullptr != val_ptr)
		{
			nlohmann::json arr = nlohmann::json::array();
			for (const auto& val : *val_ptr)
			{
				arr.push_back(basic_metadata_to_json_converter(val));
			}
			return arr;
		}
		throw std::runtime_error("No more types supported");
	}
}

