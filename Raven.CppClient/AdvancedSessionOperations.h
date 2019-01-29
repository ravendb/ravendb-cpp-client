#pragma once
#include "DocumentSessionImpl.h"
#include "DocumentsChanges.h"

namespace ravendb::client::documents::session
{
	class AdvancedSessionOperations
	{
	public:
		friend class DocumentSession;

	private:
		std::shared_ptr<DocumentSessionImpl> _session_impl;

		explicit AdvancedSessionOperations(std::shared_ptr<DocumentSessionImpl> session_impl)
			: _session_impl(session_impl)
		{}

	public:
		~AdvancedSessionOperations() = default;

		std::reference_wrapper<IDocumentStore> get_document_store() const
		{
			return _session_impl->get_document_store();
		}

		std::unordered_map<std::string, std::shared_ptr<void>>& get_external_state() const
		{
			return _session_impl->external_state;
		}

		//TODO ServerNode get_current_session_node() const;

		std::shared_ptr<http::RequestExecutor> get_request_executor() const
		{
			return _session_impl->get_request_executor();
		}

		bool has_changes() const
		{
			return _session_impl->has_changes();
		}

		int32_t get_max_number_of_requests_per_session() const
		{
			return _session_impl->max_number_of_requests_per_session;
		}

		void set_max_number_of_requests_per_session(int32_t max_requests)
		{
			_session_impl->max_number_of_requests_per_session = max_requests;
		}

		int32_t get_number_of_requests() const
		{
			return _session_impl->get_number_of_requests();
		}

		const std::string& store_identifier() const
		{
			return _session_impl->store_identifier();
		}

		bool is_use_optimistic_concurrency() const
		{
			return _session_impl->use_optimistic_concurrency;
		}

		void set_use_optimistic_concurrency(bool use_optimistic_concurrency)
		{
			_session_impl->use_optimistic_concurrency = use_optimistic_concurrency;
		}

		void clear()
		{
			_session_impl->clear();
		}

		void defer(const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands)
		{
			_session_impl->defer(commands);
		}

		template<typename T>
		void evict(std::shared_ptr<T> entity)
		{
			_session_impl->evict(entity);
		}

		template<typename T>
		std::string get_document_id(std::shared_ptr<T> entity)
		{
			return _session_impl->get_document_id(entity);
		}

		template<typename T>
		std::shared_ptr<IMetadataDictionary> get_metadata_for(std::shared_ptr<T> entity) const
		{
			return _session_impl->get_metadata_for(entity);
		}


		template<typename T>
		std::optional<std::string> get_change_vector_for(std::shared_ptr<T> entity) const
		{
			return _session_impl->get_change_vector_for(entity);
		}

		//TODO template<typename T> std::shared_ptr<IMetadataDictionary> get_counters_for(std::shared_ptr<T> entity)

		template<typename T>
		std::optional<impl::DateTimeOffset> get_last_modified_for(std::shared_ptr<T> entity) const
		{
			return _session_impl->get_last_modified_for(entity);
		}

		template<typename T>
		bool has_changed(std::shared_ptr<T> entity) const
		{
			return _session_impl->has_changed(entity);
		}

		bool is_loaded(const std::string& id) const
		{
			return _session_impl->is_loaded(id);
		}

		template<typename T>
		void ignore_changes_for(std::shared_ptr<T> entity)
		{
			_session_impl->ignore_changes_for(entity);
		}

		std::unordered_map<std::string, std::vector<DocumentsChanges>> what_changed()
		{
			return _session_impl->what_changed();
		}

		//TODO void wait_for_replication_after_save_changes();

		//TODO void wait_for_indexes_after_save_changes();

		void set_transaction_mode(TransactionMode mode)
		{
			_session_impl->set_transaction_mode(mode);
		}

		const EntityToJson& get_entity_to_json() const
		{
			return _session_impl->get_entity_to_json();
		}
	};
}
