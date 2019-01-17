#include "stdafx.h"
#include "EntityToJson.h"
#include "InMemoryDocumentSessionOperations.h"

namespace ravendb::client::documents::session
{
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
					InMemoryDocumentSessionOperations::basic_metadata_to_json_converter(metadata_it.second));
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

	nlohmann::json EntityToJson::convert_entity_to_json(std::shared_ptr<void> entity, std::shared_ptr<DocumentInfo> doc_info)
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
		if (!doc_info->to_json_converter)
		{
			throw std::runtime_error("for document id = " + doc_info->id +
				" to_json_converter is missing in the session.");
		}
		auto json = doc_info->to_json_converter(entity);

		write_metadata(json, doc_info);

		return json;
	}
}

