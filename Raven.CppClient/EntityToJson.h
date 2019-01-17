#pragma once
#include "DocumentInfo.h"

namespace ravendb::client::documents::session
{
	class EntityToJson
	{
	private:
		static void write_metadata(nlohmann::json& json, std::shared_ptr<DocumentInfo> doc_info);
		
	public:
		//static class for now
		EntityToJson() = delete;
		~EntityToJson() = delete;

		static nlohmann::json convert_entity_to_json(std::shared_ptr<void> entity, std::shared_ptr<DocumentInfo> doc_info);
	};
}
