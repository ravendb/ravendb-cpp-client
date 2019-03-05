#pragma once
#include <any>
#include <unordered_set>
#include <typeinfo>
#include <atomic>
#include "BatchOptions.h"
#include "SessionInfo.h"
#include "DocumentsById.h"
#include "EntityToJson.h"
#include "DocumentInfo.h"
#include "CompareStringsIgnoreCase.h"
#include "DateTimeOffset.h"
#include "OperationExecutor.h"
#include "ILazyOperation.h"
#include "DocumentConventions.h"
#include "utils.h"
#include "../Raven.CppClient.Tests/re_definitions.h"

namespace ravendb::client
{
	namespace json
	{
		struct BatchCommandResult;
	}
	namespace http
	{
		class RequestExecutor;
	}
	namespace documents
	{
		class DocumentStoreBase;

		namespace commands::batches
		{
			class CommandDataBase;
			enum class CommandType;
		}
		namespace session
		{
			enum class TransactionMode;
			class EntityToJson;
			struct SessionOptions;
			struct IMetadataDictionary;
			struct DocumentsChanges;

			namespace operations
			{
				class LoadOperation;
				class BatchOperation;

				namespace lazy
				{
					struct ILazyOperation;
				}
			}
		}
		namespace operations
		{
			class SessionOperationExecutor;
		}
		namespace conventions
		{
			class DocumentConventions;
		}
	}
}

namespace ravendb::client::documents::session::in_memory_document_session_operations
{
	using IdTypeAndName = std::tuple<std::string, commands::batches::CommandType, std::string>;
}

namespace std
{
	template<>
	struct hash<ravendb::client::documents::session::in_memory_document_session_operations::IdTypeAndName>
	{
		typedef ravendb::client::documents::session::in_memory_document_session_operations::IdTypeAndName argument_type;
		typedef std::size_t result_type;
		//hash calculation as in the Java client
		result_type operator()(argument_type const& a) const noexcept
		{
			const auto
				val1 = std::hash<std::string>{}(std::get<0>(a)),
				val2 = std::hash<ravendb::client::documents::commands::batches::CommandType>{}(std::get<1>(a)),
				val3 = std::hash<std::string>{}(std::get<2>(a));
			auto res = val1;
			res = 31 * res + val2;
			res = 31 * res + val3;
			return res;
		}
	};
}

namespace ravendb::client::documents::session
{	
	class InMemoryDocumentSessionOperations
	{
	public:
		friend operations::LoadOperation;
		friend operations::BatchOperation;

		static const int32_t DEFAULT_MAX_NUMBER_OF_REQUESTS_PER_SESSION = 32;

		struct  SaveChangesData
		{
			std::list<std::shared_ptr<commands::batches::CommandDataBase>> deferred_commands{};
			std::unordered_map<in_memory_document_session_operations::IdTypeAndName, std::shared_ptr<commands::batches::CommandDataBase>> deferred_commands_map{};
			std::list< std::shared_ptr<commands::batches::CommandDataBase>> session_commands{};
			std::vector<std::shared_ptr<void>> entities{};
			std::optional<commands::batches::BatchOptions> options{};

			explicit SaveChangesData(const InMemoryDocumentSessionOperations& session)
				: deferred_commands(session._deferred_commands)
				, deferred_commands_map(session._deferred_commands_map)
				, options(session._save_changes_options)
			{}
		};
	public:
		const int64_t id;

		const std::string database_name;

		int32_t max_number_of_requests_per_session = DEFAULT_MAX_NUMBER_OF_REQUESTS_PER_SESSION;

		bool use_optimistic_concurrency = true;

		const bool no_tracking;

		std::unordered_map<std::string, std::shared_ptr<void>> external_state{};
	private:
		static std::atomic_int32_t _client_session_id_counter;

		std::shared_ptr<documents::operations::OperationExecutor> _operation_executor;

		static std::atomic_int32_t _instances_counter;

		const int32_t _hash = ++_instances_counter;//unused

		TransactionMode _transaction_mode;

		int32_t _number_of_requests{};

		//TODO
		//GenerateEntityIdOnTheClient generate_entity_id_on_the_client;

		const EntityToJson _entity_to_json;

	protected:
		const int32_t _client_session_id;

		const std::shared_ptr<http::RequestExecutor> _request_executor{};

		std::vector<std::shared_ptr<operations::lazy::ILazyOperation>> _pending_lazy_operations{};

		std::unordered_map<std::shared_ptr<operations::lazy::ILazyOperation>, std::function<void()>> _on_evaluate_lazy{};

		const bool _generate_document_keys_on_store = true;

		SessionInfo _session_info;

		commands::batches::BatchOptions _save_changes_options{};

		std::unordered_set<std::shared_ptr<void>> _deleted_entities{};

		std::set<std::string, impl::utils::CompareStringsIgnoreCase> _known_missing_ids{};

		//Translate between an ID and its associated entity
		DocumentsById _documents_by_id{};

		//Translate between an ID and its associated entity
		DocumentsById _included_documents_by_id{};

		//TODO use the Data Structure that maintains the insertion order
		//hold the data required to manage the data for RavenDB's Unit of Work
		std::unordered_map<std::shared_ptr<void>, std::shared_ptr<DocumentInfo>> _documents_by_entity{};

		std::reference_wrapper<DocumentStoreBase> _document_store;

		std::list<std::shared_ptr<commands::batches::CommandDataBase>> _deferred_commands{};

		std::unordered_map<in_memory_document_session_operations::IdTypeAndName, std::shared_ptr<commands::batches::CommandDataBase>> 
			_deferred_commands_map{};

		template<typename T>
		void process_query_parameters(std::optional<std::string>& index_name,
			std::optional<std::string>& collection_name,
			std::shared_ptr<conventions::DocumentConventions> conventions);

	public:
		virtual ~InMemoryDocumentSessionOperations() = 0;

		std::reference_wrapper<IDocumentStore> get_document_store() const;

		std::shared_ptr<http::RequestExecutor> get_request_executor() const;

		std::shared_ptr<documents::operations::OperationExecutor> get_operations() const;

		//TODO ServerNode get_current_session_node();

		int32_t get_number_of_requests() const;

		size_t get_number_of_entities_in_unit_of_work() const;

		std::string store_identifier() const;

		std::shared_ptr<conventions::DocumentConventions> get_conventions() const;

		size_t get_deferred_commands_count() const;

		//TODO get_generate_entity_id_on_the_client()

		const EntityToJson& get_entity_to_json() const;

		TransactionMode get_transaction_mode() const;

		void set_transaction_mode(TransactionMode mode);

		template<typename T>
		std::shared_ptr<IMetadataDictionary> get_metadata_for(std::shared_ptr<T> entity) const;

		//TODO template<typename T> std::shared_ptr<IMetadataDictionary> get_counters_for(std::shared_ptr<T> entity)

		template<typename T>
		std::optional<std::string> get_change_vector_for(std::shared_ptr<T> entity) const;

		template<typename T>
		std::optional<impl::DateTimeOffset> get_last_modified_for(std::shared_ptr<T> entity) const;

		bool is_loaded(const std::string& id) const;

		bool is_loaded_or_deleted(const std::string& id) const;

		bool is_deleted(const std::string& id) const;

		template<typename T>
		std::optional<std::string> get_document_id(std::shared_ptr<T> entity) const;

		template<typename T>
		std::shared_ptr<T> track_entity(const DocumentInfo& document_found,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		std::shared_ptr<void> track_entity(const std::string& id,
			const nlohmann::json& document,
			const nlohmann::json& metadata,
			bool no_tracking_,
			const DocumentInfo::FromJsonConverter& from_json_converter,
			const DocumentInfo::ToJsonConverter& to_json_converter,
			const DocumentInfo::EntityUpdater& update_from_json);

		void register_external_loaded_into_the_session(std::shared_ptr<DocumentInfo> info);

		template<typename T>
		void delete_document(std::shared_ptr<T> entity);

		template<typename T>
		void delete_document(const std::string& id,
			const std::optional<std::string>& expected_change_vector = {});

		void delete_document(const std::string& id,
			const std::optional<std::string>& expected_change_vector,
			const DocumentInfo::ToJsonConverter& to_json);

		template<typename T>
		void store(std::shared_ptr<T> entity,
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		void store(std::shared_ptr<T> entity, std::string id,
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template<typename T>
		void store(std::shared_ptr<T> entity,
			std::optional<std::string> change_vector,
			std::optional<std::string> id = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		SaveChangesData prepare_for_save_changes();

		void validateClusterTransaction(const SaveChangesData& result) const;

		std::unordered_map<std::string, std::vector<DocumentsChanges>> what_changed();

		bool has_changes() const;

		template<typename T>
		bool has_changed(std::shared_ptr<T> entity) const;

		//TODO void wait_for_replication_after_save_changes();

		//TODO void wait_for_indexes_after_save_changes();

		template<typename T>
		void ignore_changes_for(std::shared_ptr<T> entity);

		template<typename T>
		void evict(std::shared_ptr<T> entity);

		void clear();

		void defer(const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands);

		void register_missing(const std::string& id);

		void register_includes(const nlohmann::json& includes);

		//TODO register_missing_includes is NOT fully implemented since it DOESN'T parse the include path
		//TODO but uses them as is (only works with simple path)
		//TODO this behaviour is NOT implemented in Java client nether.
		void register_missing_includes(const nlohmann::json& results, const nlohmann::json& includes,
			const std::vector<std::string>& include_paths);

		//TODO implement "includes" methods

		//TODO implement "counters" methods

		bool check_if_already_included(const std::vector<std::string>& ids, const std::vector<std::string>& includes);

	protected:
		InMemoryDocumentSessionOperations(DocumentStoreBase& document_store,/* UUID id,*/ SessionOptions options);

		void increment_request_count();

		//TODO virtual std::string generate_id(std::shared_ptr<void> entity) = 0;

		void store_entity_in_unit_of_work(std::optional<std::string>& id, std::shared_ptr<void> entity,
			std::optional<std::string>& change_vector, nlohmann::json metadata,
			ConcurrencyCheckMode force_concurrency_check, const DocumentInfo::ToJsonConverter& to_json,
			const DocumentInfo::EntityUpdater& update_from_json);

		void assert_is_unique_instance(std::shared_ptr<void> entity, const std::string& id) const;

		//TODO virtual ClusterTransactionOperationsBase& getClusterSession() = 0;

		bool entity_changed(const nlohmann::json& new_obj, std::shared_ptr<DocumentInfo> doc_info,
			std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>& changes_collection) const;

		void update_session_after_changes(const json::BatchCommandResult& result);

	private:
		std::shared_ptr<IMetadataDictionary> get_metadata_for_internal(std::shared_ptr<void> entity) const;

		std::optional<std::string> get_change_vector_for_internal(std::shared_ptr<void> entity) const;

		std::optional<impl::DateTimeOffset> get_last_modified_for_internal(std::shared_ptr<void> entity) const;

		std::shared_ptr<DocumentInfo> get_document_info(std::shared_ptr<void> entity) const;

		std::optional<std::string> get_document_id_internal(std::shared_ptr<void> entity) const;

		void delete_document_internal(std::shared_ptr<void> entity);

		void store_internal(std::shared_ptr<void> entity,
			std::optional<std::string> change_vector, std::optional<std::string> id,
			ConcurrencyCheckMode force_concurrency_check,
			const DocumentInfo::ToJsonConverter& to_json,
			const DocumentInfo::EntityUpdater& update_from_json,
			std::type_index type);

		//TODO void prepare_compare_exchange_entities(const SaveChangesData& result);

		static bool update_metadata_modifications(std::shared_ptr<DocumentInfo> doc_info);

		void prepare_for_entities_deletion(SaveChangesData& result,
			std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>& changes_collection);
		
		void prepare_for_entities_puts(SaveChangesData& result);
		
		static void throw_invalid_modified_document_with_deferred_command(std::shared_ptr<commands::batches::CommandDataBase> result_command);

		static void throw_invalid_deleted_document_with_deferred_command(std::shared_ptr<commands::batches::CommandDataBase> result_command);

		static void throw_no_database();

		bool has_changed_internal(std::shared_ptr<void> entity) const;

		void get_all_entities_changes(std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>& changes_collection);

		void evict_internal(std::shared_ptr<void> entity);

		void defer_internal(std::shared_ptr<commands::batches::CommandDataBase> command);

		void add_command(std::shared_ptr<commands::batches::CommandDataBase> command);
	};

	template<typename T>
	inline std::shared_ptr<IMetadataDictionary> InMemoryDocumentSessionOperations::get_metadata_for(std::shared_ptr<T> entity) const
	{
		if (!entity)
		{
			throw std::invalid_argument("Entity cannot be empty");
		}
		return get_metadata_for_internal(std::static_pointer_cast<void>(entity));
	}

	template<typename T>
	inline std::optional<std::string> InMemoryDocumentSessionOperations::get_change_vector_for(std::shared_ptr<T> entity) const
	{
		if (!entity)
		{
			throw std::invalid_argument("Entity cannot be null");
		}
		return get_change_vector_for_internal(std::static_pointer_cast<void>(entity));
	}

	template<typename T>
	inline std::optional<impl::DateTimeOffset> InMemoryDocumentSessionOperations::get_last_modified_for(std::shared_ptr<T> entity) const
	{
		if (!entity)
		{
			throw std::invalid_argument("Entity cannot be null");
		}
		return get_last_modified_for_internal(std::static_pointer_cast<void>(entity));
	}

	template<typename T>
	std::optional<std::string> InMemoryDocumentSessionOperations::get_document_id(std::shared_ptr<T> entity) const
	{
		return get_document_id_internal(std::static_pointer_cast<void>(entity));
	}

	template<typename T>
	std::shared_ptr<T> InMemoryDocumentSessionOperations::track_entity(const DocumentInfo & document_found, const std::optional<DocumentInfo::FromJsonConverter>& from_json, const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		DocumentInfo::FromJsonConverter from_json_converter =
			document_found.from_json_converter ? document_found.from_json_converter : DocumentInfo::default_from_json<T>;
		if (from_json)
			from_json_converter = *from_json;
		DocumentInfo::ToJsonConverter to_json_converter =
			document_found.to_json_converter ? document_found.to_json_converter : DocumentInfo::default_to_json<T>;
		if (to_json)
			to_json_converter = *to_json;
		DocumentInfo::EntityUpdater update_from_json = DocumentInfo::default_entity_update<T>;

		return std::static_pointer_cast<T>(track_entity(
			document_found.id, document_found.document, document_found.metadata, no_tracking,
			from_json_converter, to_json_converter, update_from_json));
	}

	template<typename T>
	void InMemoryDocumentSessionOperations::delete_document(std::shared_ptr<T> entity)
	{
		if (!entity)
		{
			throw std::invalid_argument("Entity cannot be null");
		}
		delete_document_internal(std::static_pointer_cast<void>(entity));
	}

	template<typename T>
	void InMemoryDocumentSessionOperations::delete_document(const std::string & id, const std::optional<std::string>& expected_change_vector)
	{
		delete_document(id, expected_change_vector, DocumentInfo::default_to_json<T>);
	}

	template<typename T>
	void InMemoryDocumentSessionOperations::store(std::shared_ptr<T> entity, const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		//TODO check if has id
		bool has_id = false;

		store_internal(std::static_pointer_cast<void>(entity), {}, {},
			!has_id ? ConcurrencyCheckMode::FORCED : ConcurrencyCheckMode::AUTO,
			to_json ? *to_json : DocumentInfo::default_to_json<T>,
			DocumentInfo::default_entity_update<T>,
			typeid(T));
	}

	template<typename T>
	void InMemoryDocumentSessionOperations::store(std::shared_ptr<T> entity, std::string id, const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		store_internal(std::static_pointer_cast<void>(entity), {}, std::move(id), ConcurrencyCheckMode::AUTO,
			to_json ? *to_json : DocumentInfo::default_to_json<T>,
			DocumentInfo::default_entity_update<T>, 
			typeid(T));
	}

	template<typename T>
	void InMemoryDocumentSessionOperations::store(std::shared_ptr<T> entity, std::optional<std::string> change_vector, std::optional<std::string> id, const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		store_internal(std::static_pointer_cast<void>(entity), std::move(change_vector), std::move(id),
			!change_vector ? ConcurrencyCheckMode::DISABLED : ConcurrencyCheckMode::FORCED,
			to_json ? *to_json : DocumentInfo::default_to_json<T>,
			DocumentInfo::default_entity_update<T>,
			typeid(T));
	}

	template<typename T>
	bool InMemoryDocumentSessionOperations::has_changed(std::shared_ptr<T> entity) const
	{
		return has_changed_internal(std::static_pointer_cast<void>(entity));
	}

	template<typename T>
	void InMemoryDocumentSessionOperations::ignore_changes_for(std::shared_ptr<T> entity)
	{
		get_document_info(std::static_pointer_cast<void>(entity))->ignore_changes = true;
	}

	template<typename T>
	void InMemoryDocumentSessionOperations::evict(std::shared_ptr<T> entity)
	{
		evict_internal(std::static_pointer_cast<void>(entity));
	}

	template<typename T>
	void InMemoryDocumentSessionOperations::process_query_parameters(std::optional<std::string>& index_name,
		std::optional<std::string>& collection_name, std::shared_ptr<conventions::DocumentConventions> conventions)
	{
		const bool has_index = index_name && !impl::utils::is_blank(*index_name);
		const bool has_collection = collection_name && !collection_name->empty();

		if (has_index && has_collection)
		{
			throw std::runtime_error("Parameters 'index_name' and 'collection_name' are mutually exclusive."
				" Please specify only one of them.");
		}

		if (!has_index && !has_collection)
		{
			auto collection_name_temp = conventions->get_collection_name(typeid(T));
			collection_name = impl::utils::is_blank(collection_name_temp) ?
				constants::documents::metadata::ALL_DOCUMENTS_COLLECTION : collection_name_temp;
		}
	}
}

