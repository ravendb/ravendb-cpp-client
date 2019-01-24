#pragma once
#include "DocumentsChanges.h"
#include "DocumentInfo.h"

namespace ravendb::client::json
{
	class JsonOperation
	{
	private:
		static std::string field_path_combine(const std::string& path1, const std::string& path2);

		static std::string add_index_field_path(const std::string& field_path, size_t position);

		static bool compare_values(const nlohmann::json& old_prop, const nlohmann::json& new_prop);

		static bool compare_json_array(const std::string& field_path, const std::string& id,
			const nlohmann::json& old_array, const nlohmann::json& new_array,
			std::optional<std::unordered_map<std::string, std::vector<documents::session::DocumentsChanges>>>& changes_collection,
			std::optional<std::vector<documents::session::DocumentsChanges>>& doc_changes_collection,
			const std::string& prop_name);

		static void new_change(const std::string& field_path, const std::string& name,
			const nlohmann::json& new_value, const nlohmann::json& old_value,
			std::vector<documents::session::DocumentsChanges>& doc_changes,
			documents::session::DocumentsChanges::ChangeType change);

	public:
		//static class
		JsonOperation() = delete;
		~JsonOperation() = delete;

		static bool entity_changed(const nlohmann::json& new_obj,
			std::shared_ptr<documents::session::DocumentInfo>& doc_info,
			std::optional<std::unordered_map<std::string, std::vector<documents::session::DocumentsChanges>>>& changes_collection);

		static bool compare_json(const std::string& field_path, const std::string& id,
			const nlohmann::json& original_json, const nlohmann::json& new_json,
			std::optional<std::unordered_map<std::string, std::vector<documents::session::DocumentsChanges>>>& changes_collection,
			std::optional<std::vector<documents::session::DocumentsChanges>>& doc_changes);
	};
}
