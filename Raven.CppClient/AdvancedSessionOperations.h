#pragma once
#include "DocumentSessionImpl.h"
#include "DocumentsChanges.h"
#include "EagerSessionOperations.h"

namespace ravendb::client::documents::session
{
	class AdvancedSessionOperations
	{
	public:
		friend class DocumentSession;

	private:
		std::shared_ptr<DocumentSessionImpl> _session_impl;

		explicit AdvancedSessionOperations(std::shared_ptr<DocumentSessionImpl> session_impl);

	public:
		~AdvancedSessionOperations();

		operations::lazy::LazySessionOperations lazily();

		operations::lazy::EagerSessionOperations eagerly();

		std::reference_wrapper<IDocumentStore> get_document_store() const;

		std::unordered_map<std::string, std::shared_ptr<void>>& get_external_state() const;

		//TODO ServerNode get_current_session_node() const;

		std::shared_ptr<http::RequestExecutor> get_request_executor() const;

		bool has_changes() const;

		int32_t get_max_number_of_requests_per_session() const;

		void set_max_number_of_requests_per_session(int32_t max_requests);

		int32_t get_number_of_requests() const;

		std::string store_identifier() const;

		bool is_use_optimistic_concurrency() const;

		void set_use_optimistic_concurrency(bool use_optimistic_concurrency);

		void clear();

		void defer(const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands);

		template<typename T>
		void evict(std::shared_ptr<T> entity);

		template<typename T>
		std::optional<std::string> get_document_id(std::shared_ptr<T> entity);

		template<typename T>
		std::shared_ptr<IMetadataDictionary> get_metadata_for(std::shared_ptr<T> entity) const;

		template<typename T>
		std::optional<std::string> get_change_vector_for(std::shared_ptr<T> entity) const;

		//TODO template<typename T> std::shared_ptr<IMetadataDictionary> get_counters_for(std::shared_ptr<T> entity)

		template<typename T>
		std::optional<impl::DateTimeOffset> get_last_modified_for(std::shared_ptr<T> entity) const;

		template<typename T>
		bool has_changed(std::shared_ptr<T> entity) const;

		bool is_loaded(const std::string& id) const;

		template<typename T>
		void ignore_changes_for(std::shared_ptr<T> entity);

		std::unordered_map<std::string, std::vector<DocumentsChanges>> what_changed();

		//TODO void wait_for_replication_after_save_changes();

		//TODO void wait_for_indexes_after_save_changes();

		void set_transaction_mode(TransactionMode mode);

		const EntityToJson& get_entity_to_json() const;

		std::shared_ptr<RawDocumentQuery> raw_query(const std::string& query);

		template<typename T, typename V>
		void patch(std::shared_ptr<T> entity, const std::string& path, const V& value,
			std::optional<DocumentInfo::EntityUpdater> update_from_json = {});

		template<typename V>
		void patch(const std::string& id, const std::string& path, const V& value);

		template<typename V>
		void patch(const std::string& id, const std::string& path, const V& value,
			const DocumentInfo::EntityUpdater& update_from_json);

		template<typename T, typename U>
		void patch(std::shared_ptr<T> entity, const std::string& path_to_array,
			std::function<void(JavaScriptArray<U>&)> array_adder,
			std::optional<DocumentInfo::EntityUpdater> update_from_json = {});

		template<typename U>
		void patch(const std::string& id, const std::string& path_to_array,
			const std::function<void(JavaScriptArray<U>&)>& array_adder);

		template<typename U>
		void patch(const std::string& id, const std::string& path_to_array,
			const std::function<void(JavaScriptArray<U>&)>& array_adder,
			const DocumentInfo::EntityUpdater& update_from_json);


		template<typename T, typename V>
		void increment(std::shared_ptr<T> entity, const std::string& path, const V& value_to_add,
			std::optional<DocumentInfo::EntityUpdater> update_from_json = {});

		template<typename V>
		void increment(const std::string& id, const std::string& path, const V& value_to_add);

		template<typename V>
		void increment(const std::string& id, const std::string& path, const V& value_to_add,
			const DocumentInfo::EntityUpdater& update_from_json);
	};

	template <typename T>
	void AdvancedSessionOperations::evict(std::shared_ptr<T> entity)
	{
		_session_impl->evict(entity);
	}

	template <typename T>
	std::optional<std::string> AdvancedSessionOperations::get_document_id(std::shared_ptr<T> entity)
	{
		return _session_impl->get_document_id(entity);
	}

	template <typename T>
	std::shared_ptr<IMetadataDictionary> AdvancedSessionOperations::get_metadata_for(std::shared_ptr<T> entity) const
	{
		return _session_impl->get_metadata_for(entity);
	}

	template <typename T>
	std::optional<std::string> AdvancedSessionOperations::get_change_vector_for(std::shared_ptr<T> entity) const
	{
		return _session_impl->get_change_vector_for(entity);
	}

	template <typename T>
	std::optional<impl::DateTimeOffset> AdvancedSessionOperations::get_last_modified_for(
		std::shared_ptr<T> entity) const
	{
		return _session_impl->get_last_modified_for(entity);
	}

	template <typename T>
	bool AdvancedSessionOperations::has_changed(std::shared_ptr<T> entity) const
	{
		return _session_impl->has_changed(entity);
	}

	template <typename T>
	void AdvancedSessionOperations::ignore_changes_for(std::shared_ptr<T> entity)
	{
		_session_impl->ignore_changes_for(entity);
	}

	template <typename T, typename V>
	void AdvancedSessionOperations::patch(std::shared_ptr<T> entity, const std::string& path, const V& value,
	                                      std::optional<DocumentInfo::EntityUpdater> update_from_json)
	{
		auto&& metadata = _session_impl->get_metadata_for(entity);
		auto id = std::any_cast<std::string>(metadata->get_dictionary().at(constants::documents::metadata::ID));
		patch(std::move(id), path, value,
		      update_from_json ? std::move(update_from_json).value() : DocumentInfo::default_entity_update<T>);
	}

	template <typename V>
	void AdvancedSessionOperations::patch(const std::string& id, const std::string& path, const V& value)
	{
		_session_impl->patch(id, path, value, {});
	}

	template <typename T, typename U>
	void AdvancedSessionOperations::patch(std::shared_ptr<T> entity, const std::string& path_to_array,
		std::function<void(JavaScriptArray<U>&)> array_adder,
		std::optional<DocumentInfo::EntityUpdater> update_from_json)
	{
		auto&& metadata = _session_impl->get_metadata_for(entity);
		auto id = std::any_cast<std::string>(metadata->get_dictionary().at(constants::documents::metadata::ID));
		patch(std::move(id), path_to_array, array_adder,
			update_from_json ? std::move(update_from_json).value() : DocumentInfo::default_entity_update<T>);
	}

	template <typename U>
	void AdvancedSessionOperations::patch(const std::string& id, const std::string& path_to_array,
		const std::function<void(JavaScriptArray<U>&)>& array_adder)
	{
		_session_impl->patch(id, path_to_array, array_adder, {});
	}

	template <typename U>
	void AdvancedSessionOperations::patch(const std::string& id, const std::string& path_to_array,
		const std::function<void(JavaScriptArray<U>&)>& array_adder,
		const DocumentInfo::EntityUpdater& update_from_json)
	{
		if (!update_from_json)
		{
			throw std::invalid_argument("'update_from_json' should have a target");
		}
		_session_impl->patch(id, path_to_array, array_adder, update_from_json);
	}

	template <typename V>
	void AdvancedSessionOperations::patch(const std::string& id, const std::string& path, const V& value,
		const DocumentInfo::EntityUpdater& update_from_json)
	{
		if (!update_from_json)
		{
			throw std::invalid_argument("'update_from_json' should have a target");
		}
		_session_impl->patch(id, path, value, update_from_json);
	}

	template <typename T, typename V>
	void AdvancedSessionOperations::increment(std::shared_ptr<T> entity, const std::string& path, const V& value_to_add,
		std::optional<DocumentInfo::EntityUpdater> update_from_json)
	{
		auto&& metadata = _session_impl->get_metadata_for(entity);
		auto id = std::any_cast<std::string>(metadata->get_dictionary().at(constants::documents::metadata::ID));
		increment(std::move(id), path, value_to_add,
		          update_from_json ? std::move(update_from_json).value() : DocumentInfo::default_entity_update<T>);
	}

	template <typename V>
	void AdvancedSessionOperations::increment(const std::string& id, const std::string& path, const V& value_to_add)
	{
		_session_impl->increment(id, path, value_to_add, {});
	}

	template <typename V>
	void AdvancedSessionOperations::increment(const std::string& id, const std::string& path, const V& value_to_add,
	                                          const DocumentInfo::EntityUpdater& update_from_json)
	{
		if (!update_from_json)
		{
			throw std::invalid_argument("'update_from_json' should have a target");
		}
		_session_impl->increment(id, path, value_to_add, update_from_json);
	}
}
