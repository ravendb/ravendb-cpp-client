#pragma once

namespace ravendb::client::documents::session
{
	struct DocumentsChanges
	{
		enum class JsonNodeType {
			ARRAY,
			BINARY,
			BOOLEAN,
			MISSING,
			NULL_,
			NUMBER,
			OBJECT,
			STRING
		};

		enum class ChangeType
		{
			DOCUMENT_DELETED,
			DOCUMENT_ADDED,
			FIELD_CHANGED,
			NEW_FIELD,
			REMOVED_FIELD,
			ARRAY_VALUE_CHANGED,
			ARRAY_VALUE_ADDED,
			ARRAY_VALUE_REMOVED,
			FIELD_TYPE_CHANGED,
			ENTITY_TYPE_CHANGED
		};

		nlohmann::json field_old_value{};
		nlohmann::json field_new_value{};

		JsonNodeType field_old_type{};
		JsonNodeType field_new_type{};

		ChangeType change{};
		
		std::string field_name{};
		std::string field_path{};		
	};
}
