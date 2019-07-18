#pragma once
#include <any>
#include "DocumentInfo.h"

namespace ravendb::client::documents::session
{
	class InMemoryDocumentSessionOperations;

	class EntityToJson
	{
	private:
		std::weak_ptr<InMemoryDocumentSessionOperations> _session;

		static void write_metadata(nlohmann::json& json, std::shared_ptr<DocumentInfo> doc_info);
		
	public:
		explicit EntityToJson(std::shared_ptr<InMemoryDocumentSessionOperations> session);
		~EntityToJson() = default;

		static void populate_entity(std::shared_ptr<void> entity, const std::string& id, const nlohmann::json& document,
			const DocumentInfo::EntityUpdater& update_from_json);

		static nlohmann::json convert_entity_to_json(std::shared_ptr<void> entity, std::shared_ptr<DocumentInfo> doc_info,
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		static nlohmann::json basic_metadata_to_json_converter(const std::any& object);
	};
}
