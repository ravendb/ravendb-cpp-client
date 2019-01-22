#pragma once
#include "utils.h"

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
		const std::reference_wrapper<InMemoryDocumentSessionOperations> _session;
		std::vector<std::shared_ptr<void>> _entities{};
		size_t _session_commands_count{};
		size_t _all_commands_count{};
		std::map<std::string, std::shared_ptr<DocumentInfo>, impl::utils::CompareStringsIgnoreCase> _modifications{};

		void handle_put(size_t index, const nlohmann::json& batch_result, bool is_deferred);
		
		void handle_delete(const nlohmann::json& batch_result);

		void handle_delete_internal(const nlohmann::json& batch_result, commands::batches::CommandType type);
		
		void finalize_result();

		void apply_metadata_modifications(const std::string& id, std::shared_ptr<DocumentInfo>doc_info);
		
		std::shared_ptr<DocumentInfo> get_or_add_modifications(const std::string& id,
			std::shared_ptr<DocumentInfo> doc_info, bool apply_modifications);

		static std::string get_string_field(const nlohmann::json& j, commands::batches::CommandType type, const std::string& field_name);

		static void throw_missing_field(commands::batches::CommandType type, const std::string& field_name);

		static void throw_on_empty_results();

	public:
		explicit BatchOperation(InMemoryDocumentSessionOperations& session)
			: _session(std::ref(session))
		{}

		std::optional<commands::batches::BatchCommand> create_request();

		void set_result(const json::BatchCommandResult& result);
	};
}
