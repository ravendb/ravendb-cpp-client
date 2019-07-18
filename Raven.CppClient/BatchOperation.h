#pragma once
#include <map>
#include <vector>
#include "CompareStringsLessThanIgnoreCase.h"
#include "json.hpp"

namespace ravendb::client
{
	namespace json
	{
		struct BatchCommandResult;
	}

	namespace documents
	{
		namespace commands::batches
		{
			enum class CommandType;
			class BatchCommand;
		}
		namespace session
		{
			class InMemoryDocumentSessionOperations;
			struct DocumentInfo;
		}
	}
}

namespace ravendb::client::documents::session::operations
{
	class BatchOperation
	{
	private:
		const std::weak_ptr<InMemoryDocumentSessionOperations> _session;
		std::vector<std::shared_ptr<void>> _entities{};
		size_t _session_commands_count{};
		size_t _all_commands_count{};
		//TODO add
		//InMemoryDocumentSessionOperations.SaveChangesData.ActionsToRunOnSuccess _onSuccessfulRequest;
		std::map<std::string, std::shared_ptr<DocumentInfo>, impl::utils::CompareStringsLessThanIgnoreCase> _modifications{};

	private:
		void handle_attachment_put(const nlohmann::json& batch_result);

		void handle_attachment_put_internal(const nlohmann::json& batch_result, commands::batches::CommandType type,
			const std::string& id_field_name, const std::string& attachment_name_field_name);

		void handle_attachment_delete(const nlohmann::json& batch_result);

		void handle_attachment_delete_internal(const nlohmann::json& batch_result, commands::batches::CommandType type,
			const std::string& id_field_name, const std::string& attachment_name_field_name);

		void handle_attachment_move(const nlohmann::json& batch_result);

		void handle_attachment_copy(const nlohmann::json& batch_result);

		void handle_put(size_t index, const nlohmann::json& batch_result, bool is_deferred);

		void handle_patch(const nlohmann::json& batch_result);

		void handle_delete(const nlohmann::json& batch_result);

		void handle_delete_internal(const nlohmann::json& batch_result, commands::batches::CommandType type);
		
		void finalize_result();

		void apply_metadata_modifications(const std::string& id, std::shared_ptr<DocumentInfo>doc_info);
		
		std::shared_ptr<DocumentInfo> get_or_add_modifications(const std::string& id,
			std::shared_ptr<DocumentInfo> doc_info, bool apply_modifications);

		static std::string get_string_field(const nlohmann::json& j, commands::batches::CommandType type, const std::string& field_name);

		static int64_t get_long_field(const nlohmann::json& j, commands::batches::CommandType type, const std::string& field_name);

		static void throw_missing_field(commands::batches::CommandType type, const std::string& field_name);

		static void throw_on_empty_results();

	public:
		explicit BatchOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session)
			: _session(session)
		{}

		std::optional<commands::batches::BatchCommand> create_request();

		void set_result(const json::BatchCommandResult& result);
	};
}
