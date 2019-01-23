#pragma once
#include <any>

namespace ravendb::client::documents::session
{
	struct DocumentInfo;
	class InMemoryDocumentSessionOperations;

	class EntityToJson
	{
	private:
		std::reference_wrapper<InMemoryDocumentSessionOperations> _session;

		static void write_metadata(nlohmann::json& json, std::shared_ptr<DocumentInfo> doc_info);
		
	public:
		//static class for now
		explicit EntityToJson(InMemoryDocumentSessionOperations& session);
		~EntityToJson() = default;

		static nlohmann::json convert_entity_to_json(std::shared_ptr<void> entity, std::shared_ptr<DocumentInfo> doc_info);

		static nlohmann::json basic_metadata_to_json_converter(const std::any& object);
	};
}
