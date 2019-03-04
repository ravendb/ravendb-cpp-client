#include "stdafx.h"
#include "InMemoryDocumentSessionOperations.h"
#include "SessionOperationExecutor.h"
#include "DocumentStoreBase.h"
#include "TransactionMode.h"
#include "SessionOptions.h"
#include "MetadataAsDictionary.h"
#include "DateTimeOffset.h"
#include "DocumentsChanges.h"
#include "DeleteCommandData.h"
#include "PutCommandData.h"
#include "JsonOperation.h"
#include "BatchCommandResult.h"
#include "DocumentConventions.h"

namespace ravendb::client::documents::session
{
	InMemoryDocumentSessionOperations::~InMemoryDocumentSessionOperations() = default;

	std::atomic_int32_t InMemoryDocumentSessionOperations::_client_session_id_counter{};
	std::atomic_int32_t InMemoryDocumentSessionOperations::_instances_counter{};

	std::reference_wrapper<IDocumentStore> InMemoryDocumentSessionOperations::get_document_store() const
	{
		return _document_store;
	}

	std::shared_ptr<http::RequestExecutor> InMemoryDocumentSessionOperations::get_request_executor() const
	{
		return _request_executor;
	}

	std::shared_ptr<documents::operations::OperationExecutor> InMemoryDocumentSessionOperations::get_operations() const
	{
		return _operation_executor;
	}

	int32_t InMemoryDocumentSessionOperations::get_number_of_requests() const
	{
		return _number_of_requests;
	}

	size_t InMemoryDocumentSessionOperations::get_number_of_entities_in_unit_of_work() const
	{
		return _documents_by_entity.size();
	}

	std::string InMemoryDocumentSessionOperations::store_identifier() const
	{
		std::ostringstream res;
		res << _document_store.get().get_identifier() << ";" << database_name;
		return res.str();
	}

	std::shared_ptr<conventions::DocumentConventions> InMemoryDocumentSessionOperations::get_conventions() const
	{
		return _request_executor->get_conventions();
	}

	size_t InMemoryDocumentSessionOperations::get_deferred_commands_count() const
	{
		return _deferred_commands.size();
	}

	const EntityToJson& InMemoryDocumentSessionOperations::get_entity_to_json() const
	{
		return _entity_to_json;
	}

	TransactionMode InMemoryDocumentSessionOperations::get_transaction_mode() const
	{
		return _transaction_mode;
	}

	void InMemoryDocumentSessionOperations::set_transaction_mode(TransactionMode mode)
	{
		if(mode == TransactionMode::UNSET)
		{
			throw std::invalid_argument("Cannot set the transaction mode to UNSET");
		}
		_transaction_mode = mode;
	}

	InMemoryDocumentSessionOperations::InMemoryDocumentSessionOperations(DocumentStoreBase& document_store, SessionOptions options)
		: id(0)//TODO set id	
		, database_name([&]
	{
		if (!impl::utils::is_blank(options.database))
		{
			return std::move(options.database);
		}
		if (!impl::utils::is_blank(document_store.get_database()))
		{
			return document_store.get_database();
		}
		throw_no_database();
		return std::string{};//not suppose to happen - just shut the warning
	}())
		, no_tracking(options.no_tracking)
		, _operation_executor(std::make_shared<documents::operations::SessionOperationExecutor>(*this))
		, _transaction_mode(options.transaction_mode)
		, _entity_to_json(*this)
		, _client_session_id(++_client_session_id_counter)
		, _request_executor(options.request_executor ? options.request_executor : 
			document_store.get_request_executor(database_name))
		, _session_info(_client_session_id, /*document_store.get_last_transaction_index(database_name)*/{}, options.no_caching)
		, _document_store(document_store)



	{}

	std::shared_ptr<IMetadataDictionary> InMemoryDocumentSessionOperations::get_metadata_for_internal(std::shared_ptr<void> entity) const
	{
		auto doc_info = get_document_info(entity);
		if (doc_info->metadata_instance)
		{
			return doc_info->metadata_instance;
		}

		auto metadata = json::MetadataAsDictionary::create(doc_info->metadata);
		doc_info->metadata_instance = std::static_pointer_cast<IMetadataDictionary>(metadata);
		return doc_info->metadata_instance;
	}

	std::optional<std::string> InMemoryDocumentSessionOperations::get_change_vector_for_internal(std::shared_ptr<void> entity) const
	{
		auto doc_info = get_document_info(entity);

		std::optional<std::string> change_vector{};
		impl::utils::json_utils::get_val_from_json(doc_info->metadata, constants::documents::metadata::CHANGE_VECTOR, change_vector);

		return change_vector;		
	}

	std::optional<impl::DateTimeOffset> InMemoryDocumentSessionOperations::get_last_modified_for_internal(std::shared_ptr<void> entity) const
	{
		auto doc_info = get_document_info(entity);

		std::optional<impl::DateTimeOffset> last_modified{};
		impl::utils::json_utils::get_val_from_json(doc_info->metadata, constants::documents::metadata::LAST_MODIFIED, last_modified);

		return last_modified;
	}

	std::shared_ptr<DocumentInfo> InMemoryDocumentSessionOperations::get_document_info(std::shared_ptr<void> entity) const
	{
		if (auto doc_info_it = _documents_by_entity.find(entity);
			doc_info_it != _documents_by_entity.end())
		{
			return doc_info_it->second;
		}

		//TODO get id
		throw std::runtime_error("Not implemented");
	}

	bool InMemoryDocumentSessionOperations::is_loaded(const std::string& id) const
	{
		return is_loaded_or_deleted(id);
	}

	bool InMemoryDocumentSessionOperations::is_loaded_or_deleted(const std::string& id) const
	{
		auto doc_info = _documents_by_id.find(id);
		bool is_loaded = doc_info != _documents_by_id.end() && (!doc_info->second->document.empty() || doc_info->second->entity);

		return is_loaded || is_deleted(id) || _included_documents_by_id.find(id) != _included_documents_by_id.end();
	}

	bool InMemoryDocumentSessionOperations::is_deleted(const std::string& id) const
	{
		return _known_missing_ids.find(id) != _known_missing_ids.end();
	}
	std::optional<std::string> InMemoryDocumentSessionOperations::get_document_id_internal(std::shared_ptr<void> entity) const
	{
		if(!entity)
		{
			return {};
		}
		if(auto doc_info_it = _documents_by_entity.find(entity);
			doc_info_it != _documents_by_entity.end())
		{
			return doc_info_it->second->id;
		}
		return {};
	}

	void InMemoryDocumentSessionOperations::increment_request_count()
	{
		if (++_number_of_requests > max_number_of_requests_per_session)
		{
			std::ostringstream error;
			error << "The maximum number of requests " << max_number_of_requests_per_session <<
				" allowed for this session has been reached." <<
				"Raven limits the number of remote calls that a session is allowed to make as an early warning system. Sessions are expected to be short lived, and "
				"Raven provides facilities like load(std::vector<std::string> keys) to load multiple documents at once and batch saves (call save_changes() only once)."
				"You can increase the limit by setting DocumentConvention.max_number_of_requests_per_session or 'max_number_of_requests_per_session', but it is"
				"advisable that you'll look into reducing the number of remote calls first, since that will speed up your application significantly and result in a"
				"more responsive application.";
			throw std::runtime_error(error.str());
		}
	}

	std::shared_ptr<void> InMemoryDocumentSessionOperations::track_entity(const std::string& id, const nlohmann::json& document,
		const nlohmann::json& metadata, bool no_tracking_,
		const DocumentInfo::FromJsonConverter& from_json_converter,
		const DocumentInfo::ToJsonConverter& to_json_converter,
		const DocumentInfo::EntityUpdater& update_from_json)
	{
		if (!from_json_converter)
		{
			throw std::invalid_argument("from_json_converter must have a target");
		}
		if (!to_json_converter)
		{
			throw std::invalid_argument("to_json_converter must have a target");
		}
		if (!update_from_json)
		{
			throw std::invalid_argument("update_from_json must have a target");
		}

		// if noTracking is session-wide then we want to override the passed argument
		no_tracking_ = this->no_tracking || no_tracking_;

		if (id.empty())
		{
			//TODO something
			throw std::runtime_error("Not implemented");
		}


		if (auto doc_info_it = _documents_by_id.find(id);
			doc_info_it != _documents_by_id.end())
		{// the local instance may have been changed, we adhere to the current Unit of Work
		 // instance, and return that, ignoring anything new.
			if (!doc_info_it->second->entity)
			{
				doc_info_it->second->entity = from_json_converter(document);
			}
			if (!no_tracking_)
			{
				_included_documents_by_id.erase(id);
				_documents_by_entity.insert_or_assign(doc_info_it->second->entity, doc_info_it->second);
			}
			doc_info_it->second->from_json_converter = from_json_converter;
			doc_info_it->second->to_json_converter = to_json_converter;
			doc_info_it->second->update_from_json = update_from_json;
			return doc_info_it->second->entity;
		}

		if(auto doc_info_it = _included_documents_by_id.find(id);
			doc_info_it != _included_documents_by_id.end())
		{
			auto doc_info = doc_info_it->second;
			if (!doc_info->entity)
			{
				doc_info->entity = from_json_converter(document);
			}
			if (!no_tracking_)
			{
				_included_documents_by_id.erase(id);
				_documents_by_id.insert_or_assign(doc_info->id, doc_info);
				_documents_by_entity.insert_or_assign(doc_info->entity, doc_info);
			}
			doc_info->from_json_converter = from_json_converter;
			doc_info->to_json_converter = to_json_converter;
			doc_info->update_from_json = update_from_json;
			return doc_info->entity;
		}

		auto entity = from_json_converter(document);

		std::string change_vector{};
		if (!impl::utils::json_utils::get_val_from_json(metadata, constants::documents::metadata::CHANGE_VECTOR, change_vector))
		{
			throw std::invalid_argument("Document " + id + "must have Change Vector");
		}

		if (!no_tracking)
		{
			auto doc_info = std::make_shared<DocumentInfo>();
			doc_info->id = id;
			doc_info->document = document;
			doc_info->metadata = metadata;
			doc_info->entity = entity;
			doc_info->change_vector = change_vector;
			doc_info->from_json_converter = from_json_converter;
			doc_info->to_json_converter = to_json_converter;
			doc_info->update_from_json = update_from_json;

			_documents_by_id.insert({ doc_info->id, doc_info });
			_documents_by_entity.insert({ entity, doc_info });
		}
		return entity;
	}

	void InMemoryDocumentSessionOperations::register_external_loaded_into_the_session(std::shared_ptr<DocumentInfo> info)
	{
		if (no_tracking)
		{
			return;
		}

		if (auto existing_doc_info_it = _documents_by_id.find(info->id);
			existing_doc_info_it != _documents_by_id.end())
		{
			if (existing_doc_info_it->second->entity == info->entity)
			{
				return;
			}
			throw std::runtime_error("The document " + info->id + " is already in the session with a different entity instance.");
		}

		if (auto existing_doc_info_it = _documents_by_entity.find(info->entity);
			existing_doc_info_it != _documents_by_entity.end())
		{
			if (impl::utils::CompareStringsIgnoreCase()(existing_doc_info_it->second->id, info->id))
			{
				return;
			}
			std::ostringstream message{};
			message << "Attempted to load an entity with id " << info->id <<
				", but the entity instance already exists in the session with id: " << existing_doc_info_it->second->id;
			throw std::runtime_error(message.str());
		}

		_documents_by_entity.insert({ info->entity, info });
		_documents_by_id.insert({ info->id, info });
		_included_documents_by_id.erase(info->id);
	}

	void InMemoryDocumentSessionOperations::delete_document_internal(std::shared_ptr<void> entity)
	{
		if(auto doc_info_it = _documents_by_entity.find(entity);
			doc_info_it == _documents_by_entity.end())
		{
			throw std::runtime_error("This entity is not associated with the session, cannot delete unknown entity instance");
		}else
		{
			_deleted_entities.insert(entity);
			_included_documents_by_id.erase(doc_info_it->second->id);
			//TODO take care of counters
			_known_missing_ids.insert(doc_info_it->second->id);
		}
	}

	void InMemoryDocumentSessionOperations::delete_document(const std::string& id, 
		const std::optional<std::string>& expected_change_vector,
		const DocumentInfo::ToJsonConverter& to_json)
	{
		if (impl::utils::is_blank(id))
		{
			throw std::invalid_argument("Id cannot be blank");
		}

		std::string change_vector{};
		if (auto doc_info_it = _documents_by_id.find(id);
			doc_info_it != _documents_by_id.end())
		{
			auto doc_info = doc_info_it->second;
			if (!to_json && !doc_info->to_json_converter)
			{
				throw std::invalid_argument("for document id = " + id + " to_json is not provided "
					"and missing in the session. "
					"Use template delete_document() for default to_json serializer.");
			}

			nlohmann::json new_obj = EntityToJson::convert_entity_to_json(doc_info->entity, doc_info, to_json);
			if(auto empty_changes = std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>();
				doc_info->entity && entity_changed(new_obj, doc_info, empty_changes))
			{
				throw std::runtime_error("Can't delete changed entity using identifier. "
					"Use template<typename T>void delete_document(std::shared_ptr<T> typed_entity) instead.");
			}

			if (doc_info->entity)
			{
				_documents_by_entity.erase(doc_info->entity);
			}
			_documents_by_id.erase(id);
			change_vector = doc_info->change_vector;
		}

		_known_missing_ids.insert(id);
		if (!use_optimistic_concurrency)
		{
			change_vector = "";
		}

		//TODO take care of counters

		defer({ std::make_shared<commands::batches::DeleteCommandData>
			(id, expected_change_vector ? *expected_change_vector : change_vector) });
	}

	void InMemoryDocumentSessionOperations::store_internal(std::shared_ptr<void> entity,
		std::optional<std::string> change_vector, std::optional<std::string> id,
		ConcurrencyCheckMode force_concurrency_check,
		const DocumentInfo::ToJsonConverter& to_json,
		const DocumentInfo::EntityUpdater& update_from_json,
		std::type_index type)
	{
		if (no_tracking)
		{
			throw std::runtime_error("Cannot store entity. Entity tracking is disabled in this session.");
		}
		if (!entity)
		{
			throw std::runtime_error("Entity cannot be null");
		}
		if (!to_json)
		{
			throw std::invalid_argument("to_json must have a target");
		}
		if (!update_from_json)
		{
			throw std::invalid_argument("update_from_json must have a target");
		}

		if (auto doc_info_it = _documents_by_entity.find(entity);
			doc_info_it != _documents_by_entity.end())
		{
			auto doc_info = doc_info_it->second;
			if (change_vector)
			{
				doc_info->change_vector = std::move(change_vector).value();
			}

			doc_info->concurrency_check_mode = force_concurrency_check;

			if (!doc_info->to_json_converter)
			{
				doc_info->to_json_converter = to_json;
			}
			return;
		}

		if (!id)
		{
			//TODO generate id or
			throw std::runtime_error("Not implemented");
		}

		if (auto it = _deferred_commands_map.find(
			in_memory_document_session_operations::IdTypeAndName{ *id,
				commands::batches::CommandType::CLIENT_ANY_COMMAND, {} });
			it != _deferred_commands_map.end())
		{
			throw std::runtime_error("Can't store document, there is a deferred command registered for this document in the session. Document id: " + *id);
		}
		if (auto it = _deleted_entities.find(entity);
			it != _deleted_entities.end())
		{
			throw std::runtime_error("Can't store object, it was already deleted in this session.  Document id: " + *id);
		}

		// we make the check here even if we just generated the ID
		// users can override the ID generation behavior, and we need
		// to detect if they generate duplicates.
		assert_is_unique_instance(entity, id ? *id : std::string());

		
		
		//TODO generate and set collection name

		auto metadata = get_conventions()->get_serialized_type(type)->at(constants::documents::metadata::KEY);

		auto& collection_name = metadata.at(constants::documents::metadata::COLLECTION);
		if(collection_name.is_null())
		{
			collection_name = get_conventions()->get_collection_name(type);
		}

		auto& cpp_type = metadata.at(constants::documents::metadata::RAVEN_C_PLUS_PLUS_TYPE);
		if(cpp_type.is_null())
		{
			cpp_type = get_conventions()->get_c_plus_plus_class_name(type);
		}


	//TODO erase later when above is functional and tested
		////this part is unique to C++ : metadata check from the entity serialized
		////TODO probably would be transferred to DocumentConventions 
		//auto temp_doc_info = std::make_shared<DocumentInfo>();
		//temp_doc_info->to_json_converter = to_json;
		//auto document = EntityToJson::convert_entity_to_json(entity, temp_doc_info);
		
		//metadata = document[constants::documents::metadata::KEY];
		//if(metadata.find(constants::documents::metadata::COLLECTION) == metadata.end())
		//{
		//	std::ostringstream collection{};
		//	std::string_view type_name = type.name();
		//	collection << type_name.substr(type_name.find_last_of(':') + 1);
		//	impl::utils::json_utils::set_val_to_json(metadata, constants::documents::metadata::COLLECTION, collection.str());			
		//}
		//if (id)
		//{
		//	impl::utils::json_utils::set_val_to_json(metadata, constants::documents::metadata::ID, *id);
		//}


		if (id)
		{
			_known_missing_ids.erase(*id);
		}

		store_entity_in_unit_of_work(id, entity, change_vector, std::move(metadata), force_concurrency_check, to_json, update_from_json);
	}

	void InMemoryDocumentSessionOperations::store_entity_in_unit_of_work(std::optional<std::string>& id,
		std::shared_ptr<void> entity, std::optional<std::string>& change_vector, nlohmann::json metadata,
		ConcurrencyCheckMode force_concurrency_check, const DocumentInfo::ToJsonConverter& to_json,
		const DocumentInfo::EntityUpdater& update_from_json)
	{
		if (!to_json)
		{
			throw std::invalid_argument("to_json must have a target");
		}
		if (!update_from_json)
		{
			throw std::invalid_argument("update_from_json must have a target");
		}

		_deleted_entities.erase(entity);
		if (id)
		{
			_known_missing_ids.erase(*id);
		}

		auto doc_info = std::make_shared<DocumentInfo>();
		if (id)
		{
			doc_info->id = *id;
		}
		doc_info->metadata = std::move(metadata);
		if (change_vector)
		{
			doc_info->change_vector = std::move(change_vector).value();
		}
		doc_info->concurrency_check_mode = force_concurrency_check;
		doc_info->entity = entity;
		doc_info->to_json_converter = to_json;
		doc_info->update_from_json = update_from_json;
		doc_info->new_document = true;

		_documents_by_entity.insert({ entity, doc_info });
		if (id)
		{
			_documents_by_id.insert({ *id, doc_info });
		}
	}

	void InMemoryDocumentSessionOperations::assert_is_unique_instance(std::shared_ptr<void> entity, const std::string& id) const
	{
		if (id.empty() || id.back() == '|' || id.back() == '/')
		{
			return;
		}
		if (auto doc_info_it = _documents_by_id.find(id);
			doc_info_it == _documents_by_id.end() || doc_info_it->second == entity)
		{
			return;
		}
		throw std::runtime_error("Attempted to associate a different object with id '" + id + "'.");
	}

	InMemoryDocumentSessionOperations::SaveChangesData InMemoryDocumentSessionOperations::prepare_for_save_changes()
	{
		auto result = SaveChangesData{ *this };

		_deferred_commands.clear();
		_deferred_commands_map.clear();

		auto empty_changes_collection = std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>{};
		prepare_for_entities_deletion(result, empty_changes_collection);
		prepare_for_entities_puts(result);

		//TODO prepare_compare_exchange_entities(result);

		if (!_deferred_commands.empty())
		{// this allow on_before_store to call defer during the call to include
		// additional values during the same save_changes call
			result.deferred_commands.insert(result.deferred_commands.end(),
				_deferred_commands.cbegin(), _deferred_commands.cend());
			result.deferred_commands_map.insert(_deferred_commands_map.cbegin(), _deferred_commands_map.cend());
		}

		_deferred_commands.clear();
		_deferred_commands_map.clear();

		for (auto& deferred_command : result.deferred_commands)
		{
			deferred_command->on_before_save_changes(*this);
		}
		return result;
	}

	void InMemoryDocumentSessionOperations::validateClusterTransaction(const SaveChangesData& result) const
	{
		if(_transaction_mode != TransactionMode::CLUSTER_WIDE)
		{
			return;
		}
		if(use_optimistic_concurrency)
		{
			std::ostringstream message{};
			message << "useOptimisticConcurrency is not supported with TransactionMode set to " <<
				nlohmann::json(TransactionMode::CLUSTER_WIDE).get<std::string>();
			throw std::runtime_error(message.str());
		}

		for(const auto& command_data : result.session_commands )
		{
			switch (command_data->get_type())
			{
			case commands::batches::CommandType::PUT:
			case commands::batches::CommandType::DELETE_:
				if(!command_data->get_change_vector().empty())
				{
					std::ostringstream message{};
					message << "Optimistic concurrency for " << command_data->get_id() <<
						" is not supported when using a cluster transaction";
				}
				break;
			case commands::batches::CommandType::COMPARE_EXCHANGE_DELETE:
			case  commands::batches::CommandType::COMPARE_EXCHANGE_PUT:
				break;
			default:
			{
				std::ostringstream message{};
				message << "The command '" << nlohmann::json(command_data->get_type()).get<std::string>() <<
					"' is not supported in a cluster session.";
			}
			}
		}
	}

	bool InMemoryDocumentSessionOperations::update_metadata_modifications(std::shared_ptr<DocumentInfo> doc_info)
	{
		bool dirty = false;
		if (doc_info->metadata_instance)
		{
			if (doc_info->metadata_instance->is_dirty())
			{
				dirty = true;
			}
			for (auto& [key, value] : doc_info->metadata_instance->get_dictionary())
			{
				//MetadataAsDictionary weak pointer
				using MAD_wptr = std::weak_ptr<json::MetadataAsDictionary>;
				if (!value.has_value() ||
					std::any_cast<MAD_wptr>(&value) != nullptr &&
					std::any_cast<MAD_wptr>(value).lock()->is_dirty())
				{
					dirty = true;
				}
				impl::utils::json_utils::set_val_to_json(doc_info->metadata,
					key, EntityToJson::basic_metadata_to_json_converter(value));
			}
		}
		return dirty;
	}

	void InMemoryDocumentSessionOperations::prepare_for_entities_deletion(SaveChangesData& result,
		std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>& changes_collection)
	{
		for (auto& deleted_entity : _deleted_entities)
		{
			std::shared_ptr<DocumentInfo> doc_info{};
			if (auto doc_info_it = _documents_by_entity.find(deleted_entity);
				doc_info_it == _documents_by_entity.end())
			{
				continue;
			}
			else
			{
				doc_info = doc_info_it->second;
			}
			if (changes_collection)
			{
				DocumentsChanges change{};
				change.field_new_value = std::string{};
				change.field_old_value = std::string{};
				change.change = DocumentsChanges::ChangeType::DOCUMENT_DELETED;

				std::vector<DocumentsChanges> doc_changes{ std::move(change) };
				changes_collection->insert({doc_info->id, std::move(doc_changes)});
			}else
			{
				if (auto command_it = result.deferred_commands_map.find(
					in_memory_document_session_operations::IdTypeAndName
					{ doc_info->id, commands::batches::CommandType::CLIENT_ANY_COMMAND, "" });
					command_it != result.deferred_commands_map.end())
				{
					// here we explicitly want to throw for all types of deferred commands, if the document
					// is being deleted, we never want to allow any other operations on it
					throw_invalid_deleted_document_with_deferred_command(command_it->second);
				}
				std::string change_vector{};

				auto doc_info_it = _documents_by_id.find(doc_info->id);
				auto doc_info_from_id = doc_info_it != _documents_by_id.end() ? doc_info_it->second : nullptr;

				if (doc_info_from_id)
				{
					change_vector = doc_info_from_id->change_vector;
					if (doc_info_from_id->entity)
					{
						_documents_by_entity.erase(doc_info_from_id->entity);
						result.entities.emplace_back(doc_info_from_id->entity);
					}
					_documents_by_id.erase(doc_info_from_id->id);
				}
				if (!use_optimistic_concurrency)
				{
					change_vector.clear();
				}
				//TODO call EventHelper.invoke()

				result.session_commands.push_back(std::make_shared<commands::batches::DeleteCommandData>
					(doc_info->id, change_vector));
			}
		}
		if (!changes_collection)
		{
			_deleted_entities.clear();
		}
	}

	void InMemoryDocumentSessionOperations::prepare_for_entities_puts(SaveChangesData& result)
	{
		for (auto& [entity, doc_info] : _documents_by_entity)
		{
			if (doc_info->ignore_changes)
			{
				continue;
			}

			bool dirty_metadata = update_metadata_modifications(doc_info);

			nlohmann::json document = EntityToJson::convert_entity_to_json(entity, doc_info);

			if (auto empty_changes_collection = std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>{};
				!dirty_metadata && !entity_changed(document, doc_info, empty_changes_collection))
			{
				continue;
			}

			if (auto command_it = result.deferred_commands_map.find(
				{ doc_info->id, commands::batches::CommandType::CLIENT_MODIFY_DOCUMENT_COMMAND, {} });
				command_it != result.deferred_commands_map.end())
			{
				throw_invalid_modified_document_with_deferred_command(command_it->second);
			}

			//TODO use onBeforeStore

			doc_info->new_document = false;
			result.entities.push_back(entity);
			if (!doc_info->id.empty())
			{
				_documents_by_id.erase(doc_info->id);
			}

			doc_info->document = document;
			std::string change_vector{};
			if (use_optimistic_concurrency)
			{
				if (doc_info->concurrency_check_mode != ConcurrencyCheckMode::DISABLED)
				{// if the user didn't provide a change vector, we'll test for an empty one
					change_vector = doc_info->change_vector;
				}
			}
			else if (doc_info->concurrency_check_mode == ConcurrencyCheckMode::FORCED)
			{
				change_vector = doc_info->change_vector;
			}

			result.session_commands.push_back(std::make_shared<commands::batches::PutCommandData>
				(doc_info->id, std::move(change_vector), std::move(document)));
		}
	}

	void InMemoryDocumentSessionOperations::throw_invalid_modified_document_with_deferred_command(std::shared_ptr<commands::batches::CommandDataBase> result_command)
	{
		std::ostringstream error_msg{};
		error_msg << "Cannot perform save because document " << result_command->get_id() <<
			" has been modified by the session and is also taking part in deferred " <<
			nlohmann::json(result_command->get_type()).get<std::string>() << " command";

		throw std::runtime_error(error_msg.str());
	}

	void InMemoryDocumentSessionOperations::throw_invalid_deleted_document_with_deferred_command(std::shared_ptr<commands::batches::CommandDataBase> result_command)
	{
		std::ostringstream error_msg{};
		error_msg << "Cannot perform save because document " << result_command->get_id() <<
			" has been deleted by the session and is also taking part in deferred " <<
			nlohmann::json(result_command->get_type()).get<std::string>() << " command";

		throw std::runtime_error(error_msg.str());
	}

	void InMemoryDocumentSessionOperations::throw_no_database()
	{
		throw std::invalid_argument("Cannot open a Session without specifying a name of a database "
			"to operate on. Database name can be passed as an argument when Session is"
			" being opened or default database can be defined using DocumentStore.database");
	}

	bool InMemoryDocumentSessionOperations::entity_changed(const nlohmann::json& new_obj,
		std::shared_ptr<DocumentInfo> doc_info,
		std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>& changes_collection) const
	{
		return json::JsonOperation::entity_changed(new_obj, doc_info, changes_collection);
	}

	std::unordered_map<std::string, std::vector<DocumentsChanges>> InMemoryDocumentSessionOperations::what_changed()
	{
		std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>> changes_collection{};
		changes_collection.emplace();

		auto unused = SaveChangesData(*this);
		prepare_for_entities_deletion(unused, changes_collection);
		get_all_entities_changes(changes_collection);

		return *changes_collection;
	}

	bool InMemoryDocumentSessionOperations::has_changes() const
	{
		std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>> empty_changes{};

		for(auto& [entity, doc_info] : _documents_by_entity)
		{
			auto document = EntityToJson::convert_entity_to_json(entity, doc_info);
			if(entity_changed(document, doc_info, empty_changes))
			{
				return true;
			}
		}

		return !_deleted_entities.empty();
	}

	bool InMemoryDocumentSessionOperations::has_changed_internal(std::shared_ptr<void> entity) const
	{
		std::shared_ptr<DocumentInfo> doc_info{};
		if(auto doc_info_it = _documents_by_entity.find(entity);
			doc_info_it != _documents_by_entity.end())
		{
			doc_info = doc_info_it->second;
		}else
		{
			return false;
		}

		auto document = EntityToJson::convert_entity_to_json(entity, doc_info);
		std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>> empty_collection{};
		return entity_changed(document, doc_info, empty_collection);
	}

	void InMemoryDocumentSessionOperations::get_all_entities_changes(
		std::optional<std::unordered_map<std::string, std::vector<DocumentsChanges>>>& changes_collection)
	{
		if(!changes_collection)
		{
			changes_collection.emplace();
		}

		for(auto& [id, doc_info] : _documents_by_id)
		{
			update_metadata_modifications(doc_info);
			auto new_obj = EntityToJson::convert_entity_to_json(doc_info->entity, doc_info);
			entity_changed(new_obj, doc_info, changes_collection);
		}
	}

	void InMemoryDocumentSessionOperations::evict_internal(std::shared_ptr<void> entity)
	{
		if(auto doc_info_it = _documents_by_entity.find(entity);
			doc_info_it != _documents_by_entity.end())
		{
			_documents_by_id.erase(doc_info_it->second->id);
			//TODO take care of counters
		}

		_documents_by_entity.erase(entity);
		_deleted_entities.erase(entity);
	}

	void InMemoryDocumentSessionOperations::clear()
	{
		_documents_by_entity.clear();
		_deleted_entities.clear();
		_documents_by_id.clear();
		_known_missing_ids.clear();
		//TODO take care of counters
	}

	void InMemoryDocumentSessionOperations::defer(const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands)
	{
		_deferred_commands.insert(_deferred_commands.end(), commands.begin(), commands.end());
		for (auto& command : commands)
		{
			defer_internal(command);
		}
	}

	void InMemoryDocumentSessionOperations::defer_internal(std::shared_ptr<commands::batches::CommandDataBase> command)
	{
		if (command->get_type() == commands::batches::CommandType::BATCH_PATCH)
		{
			//TODO complete
			throw std::runtime_error("not implemented");
		}
		add_command(command);
	}

	void InMemoryDocumentSessionOperations::add_command(std::shared_ptr<commands::batches::CommandDataBase> command)
	{
		_deferred_commands_map.insert_or_assign(in_memory_document_session_operations::IdTypeAndName
			(command->get_id(), command->get_type(), command->get_name()), command);
		_deferred_commands_map.insert_or_assign(in_memory_document_session_operations::IdTypeAndName
			(command->get_id(), commands::batches::CommandType::CLIENT_ANY_COMMAND, {}), command);

		switch (command->get_type())
		{
		case commands::batches::CommandType::ATTACHMENT_PUT:
			break;
		case commands::batches::CommandType::ATTACHMENT_DELETE:
			break;
		case commands::batches::CommandType::ATTACHMENT_COPY:
			break;
		case commands::batches::CommandType::ATTACHMENT_MOVE:
			break;
		case commands::batches::CommandType::COUNTERS:
			break;
		default:
			_deferred_commands_map.insert_or_assign(in_memory_document_session_operations::IdTypeAndName
			(command->get_id(), commands::batches::CommandType::CLIENT_MODIFY_DOCUMENT_COMMAND, {}), command);
		}
	}

	void InMemoryDocumentSessionOperations::register_missing(const std::string& id)
	{
		if(no_tracking)
		{
			return;
		}
		_known_missing_ids.insert(id);
	}

	void InMemoryDocumentSessionOperations::register_includes(const nlohmann::json& includes)
	{
		if(!includes.is_object())
		{
			throw std::invalid_argument("'includes' should be of the type 'object'");
		}

		if(no_tracking || includes.empty())
		{
			return;
		}

		for(const auto& [key, val] : static_cast<const nlohmann::json::object_t&>(includes))
		{
			if(val.is_null())
			{
				continue;
			}
			auto doc_info = std::make_shared<DocumentInfo>(val);
			//TODO tryGetConflict(doc_info.metadata);

			_included_documents_by_id.insert_or_assign(doc_info->id, doc_info);
		}
	}

	void InMemoryDocumentSessionOperations::register_missing_includes(const nlohmann::json& results, const nlohmann::json& includes,
		const std::vector<std::string>& include_paths)
	{
		if(!results.is_array() || !includes.is_object())
		{
			throw std::invalid_argument("'results' should be of the type 'array' and 'includes' of the type 'object'");
		}

		if(no_tracking)
		{
			return;
		}

		if(include_paths.empty())
		{
			return;
		}

		for(const auto& result : static_cast<const nlohmann::json::array_t&>(results))
		{
			for(const auto& include : include_paths)
			{
				//TODO this is PARTIAL implementation of the C# code
				if(constants::documents::indexing::fields::DOCUMENT_ID_FIELD_NAME == include)
				{
					continue;
				}

				{
					std::string id{};
					if(auto id_it = result.find(include);//the path is NOT parsed !
						id_it != result.end())
					{
						id = *id_it;
					}
					if(id.empty() || is_loaded(id))
					{
						continue;
					}

					if(auto document_it = includes.find(id);
						document_it != includes.end())
					{
						auto&& document = *document_it;

						auto&& metadata = document.at(constants::documents::metadata::KEY);
						//TODO  if (JsonExtensions.tryGetConflict(metadata)) {
						//return;
						//}
					}
					register_missing(id);
				}
			}
		}
	}

	bool InMemoryDocumentSessionOperations::check_if_already_included(const std::vector<std::string>& ids,
		const std::vector<std::string>& includes)
	{
		for(const auto& id : ids)
		{
			if(_known_missing_ids.find(id) != _known_missing_ids.end())
			{
				continue;
			}

			std::shared_ptr<DocumentInfo> doc_info{};
			if(auto doc_info_it = _documents_by_id.find(id);
				doc_info_it == _documents_by_id.end())
			{
				if(auto doc_info_it2 = _included_documents_by_id.find(id);
					doc_info_it2 == _included_documents_by_id.end())
				{
					return false;
				}else
				{
					doc_info = doc_info_it2->second;
				}
			}else
			{
				doc_info = doc_info_it->second;
			}

			if(!doc_info->entity)
			{
				return false;
			}

			for(const auto& include : includes)
			{
				//TODO this is PARTIAL implementation of the JAVA code
				
			}
		}
		return true;
	}


	void InMemoryDocumentSessionOperations::update_session_after_changes(const json::BatchCommandResult& result)
	{
		auto&& returned_transaction_index = result.transaction_index;
		_document_store.get().set_last_transaction_index(database_name, returned_transaction_index);
		_session_info.last_cluster_transaction_index = returned_transaction_index;
	}


}
