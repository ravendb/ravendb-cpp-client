#pragma once
#include "RequestExecutor.h"
#include "OperationExecutor.h"
#include "TransactionMode.h"
#include "SessionInfo.h"
#include <unordered_set>
#include "DocumentInfo.h"
#include "DocumentsById.h"
#include "SessionOptions.h"
#include "Constants.h"
#include "CommandDataBase.h"
#include "BatchOptions.h"
#include "DocumentsChanges.h"
#include "PutCommandData.h"
#include "DeleteCommandData.h"

namespace ravendb::client::documents::session::operations {
	class LoadOperation;
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
		struct  SaveChangesData
		{
			std::vector<std::shared_ptr<commands::batches::CommandDataBase>> deferred_commands{};
			std::unordered_map<in_memory_document_session_operations::IdTypeAndName, std::shared_ptr<commands::batches::CommandDataBase>> deferred_commands_map{};
			std::vector< std::shared_ptr<commands::batches::CommandDataBase>> session_commands{};
			std::vector<std::shared_ptr<void>> entities{};
			std::optional<commands::batches::BatchOptions> options{};

			explicit SaveChangesData(const InMemoryDocumentSessionOperations& session)
				: deferred_commands(session._deferred_commands)
				, deferred_commands_map(session._deferred_commands_map)
				, options(session._save_changes_options)
			{}
		};

		friend operations::LoadOperation;

		virtual ~InMemoryDocumentSessionOperations() = 0;

		//BatchOptions _save_changes_options;//TODO use this
		const int32_t id;
		const std::string database_name;
		int32_t max_number_of_requests_per_session = 32;
		bool use_optimistic_concurrency = true;
		const bool no_tracking = false;

	private:
		inline static std::atomic_int32_t _client_session_id_counter{};
		ravendb::client::documents::operations::OperationExecutor _operation_executor{};
		inline static std::atomic_int32_t _instances_counter{};
		int32_t _hash = ++_instances_counter;
		TransactionMode _transaction_mode = TransactionMode::UNSET;
		int32_t _number_of_requests{};

	protected:
		const int32_t _client_session_id = ++_client_session_id_counter;
		const std::shared_ptr<http::RequestExecutor> _request_executor{};
		const bool _generate_document_keys_on_store = true;
		const SessionInfo _sessionInfo;
		std::set<std::string, impl::utils::CompareStringsToIgnoreCase> _known_missing_ids{};
		std::unordered_map<std::string, std::shared_ptr<void>> _external_state{};

		//Translate between an ID and its associated entity
		DocumentsById _documents_by_id{};
		//Translate between an ID and its associated entity
		DocumentsById _included_documents_by_id{};
		//hold the data required to manage the data for RavenDB's Unit of Work
		std::unordered_map<std::shared_ptr<void>, std::shared_ptr<DocumentInfo>> _documents_by_entity{};

		std::unordered_set<std::shared_ptr<void>> _deleted_entities{};

		commands::batches::BatchOptions _save_changes_options{};

		std::vector<std::shared_ptr<commands::batches::CommandDataBase>> _deferred_commands{};
		std::unordered_map<in_memory_document_session_operations::IdTypeAndName, std::shared_ptr<commands::batches::CommandDataBase>> 
			_deferred_commands_map{};

	public:
		std::shared_ptr<http::RequestExecutor> get_request_executor() const
		{
			return _request_executor;
		}

		int32_t get_number_of_requests() const
		{
			return _number_of_requests;
		}

		size_t get_number_of_entities_in_unit_of_work() const
		{
			return _documents_by_entity.size();
		}

		TransactionMode get_transaction_mode() const
		{
			return _transaction_mode;
		}

	private:
		void store_internal(std::shared_ptr<void> entity, std::optional<std::string> change_vector, std::optional<std::string> id, 
			ConcurrencyCheckMode force_concurrency_check, DocumentInfo::ToJsonConverter to_json)
		{
			if(no_tracking)
			{
				throw std::runtime_error("Cannot store entity. Entity tracking is disabled in this session.");
			}
			if(!entity)
			{
				throw std::runtime_error("Entity cannot be null");
			}
			if (!to_json)
			{
				throw std::invalid_argument("to_json must have a target");
			}

			if(auto doc_info_it = _documents_by_entity.find(entity);
				doc_info_it != _documents_by_entity.end())
			{
				auto doc_info = doc_info_it->second;
				if(change_vector)
				{
					doc_info->change_vector = std::move(change_vector).value();
					doc_info->concurrency_check_mode = force_concurrency_check;	
				}
				if(!doc_info->to_json_converter)
				{
					doc_info->to_json_converter = to_json;
				}
				return;
			}

			if(!id)
			{
				//TODO generate id or
				throw std::exception{};
			}

			//TODO throw is there is a deferred command registered for this document in the session

			if(auto it = _deleted_entities.find(entity);
				it != _deleted_entities.end())
			{
				throw std::runtime_error("Can't store object, it was already deleted in this session.  Document id: " + *id);
			}

			//TODO assertNoNonUniqueInstance

			nlohmann::json metadata{};
			//TODO generate and set collection name
			if(id)
			{
				_known_missing_ids.erase(*id);
			}

			store_entity_in_unit_of_work(id, entity, change_vector, std::move(metadata), force_concurrency_check, to_json);
		}

		void prepare_for_entities_deletion(SaveChangesData& result, 
			std::optional<std::unordered_map<std::string, DocumentsChanges>>& changes)
		{
			for(auto& deleted_entity : _deleted_entities)
			{
				std::shared_ptr<DocumentInfo> doc_info;
				if(auto doc_info_it = _documents_by_entity.find(deleted_entity);
					doc_info_it == _documents_by_entity.end())
				{
					continue;
				}else
				{
					doc_info = doc_info_it->second;
				}
				if(changes)
				{
					//TODO complete
				}else
				{
					if(auto command_it = result.deferred_commands_map.find(
						in_memory_document_session_operations::IdTypeAndName{ doc_info->id,
							commands::batches::CommandType::CLIENT_ANY_COMMAND, "" });
						command_it != result.deferred_commands_map.end())
					{
						// here we explicitly want to throw for all types of deferred commands, if the document
						// is being deleted, we never want to allow any other operations on it
						throw_invalid_deleted_document_with_deferred_command(command_it->second);
					}
					std::string change_vector{};

					auto doc_info_it = _documents_by_id.find(doc_info->id);
					doc_info = doc_info_it != _documents_by_id.end() ? doc_info_it->second : nullptr;

					if(doc_info)
					{
						change_vector = doc_info->change_vector;
						if(doc_info->entity)
						{
							_documents_by_entity.erase(doc_info->entity);
							result.entities.emplace_back(doc_info->entity);
						}
						_documents_by_id.erase(doc_info->id);
					}
					if(!use_optimistic_concurrency)
					{
						change_vector.clear();
					}
					//TODO call EventHelper.invoke()

					result.session_commands.push_back(std::make_shared<commands::batches::DeleteCommandData>
						(doc_info->id, change_vector));
				}
			}
			if(!changes)
			{
				_deleted_entities.clear();
			}
		}

		void prepare_for_entities_puts(SaveChangesData& result)
		{
			for(auto& entity_docinfo_pair : _documents_by_entity)
			{
				if(entity_docinfo_pair.second->ignore_changes)
				{
					continue;
				}

				//TODO bool dirty_metadata = update_metadata_modifications(entity.second);
				nlohmann::json document = entity_docinfo_pair.second->to_json_converter(entity_docinfo_pair.first);

				entity_docinfo_pair.second->new_document = false;
				result.entities.push_back(entity_docinfo_pair.first);
				if(!entity_docinfo_pair.second->id.empty())
				{
					_documents_by_id.erase(entity_docinfo_pair.second->id);
				}

				entity_docinfo_pair.second->document = document;
				std::string change_vector{};
				if(use_optimistic_concurrency)
				{
					if(entity_docinfo_pair.second->concurrency_check_mode != ConcurrencyCheckMode::DISABLED)
					{// if the user didn't provide a change vector, we'll test for an empty one
						change_vector = !entity_docinfo_pair.second->change_vector.empty() ? entity_docinfo_pair.second->change_vector : "";
					}
				} else if(entity_docinfo_pair.second->concurrency_check_mode == ConcurrencyCheckMode::FORCED)
				{
					change_vector = entity_docinfo_pair.second->change_vector;
				}

				result.session_commands.push_back(std::make_shared<commands::batches::PutCommandData>
					(entity_docinfo_pair.second->id, std::move(change_vector), std::move(document)));
			}
		}


		void defer_internal(std::shared_ptr<commands::batches::CommandDataBase> command)
		{
			if(command->get_type() == commands::batches::CommandType::BATCH_PATCH)
			{
				//TODO complete
			}

			add_command(command);
		}

		void add_command(std::shared_ptr<commands::batches::CommandDataBase> command)
		{
			_deferred_commands_map.insert({in_memory_document_session_operations::IdTypeAndName
				(command->get_id(), command->get_type(), command->get_name()), command});
			_deferred_commands_map.insert({ in_memory_document_session_operations::IdTypeAndName
				(command->get_id(), commands::batches::CommandType::CLIENT_ANY_COMMAND, {}), command });

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
				_deferred_commands_map.insert({ in_memory_document_session_operations::IdTypeAndName
				(command->get_id(), commands::batches::CommandType::CLIENT_MODIFY_DOCUMENT_COMMAND, {}), command });
			}
		}


		static void throw_invalid_deleted_document_with_deferred_command(std::shared_ptr<commands::batches::CommandDataBase> result_command)
		{
			std::ostringstream error_msg{};
			error_msg << "Cannot perform save because document " << result_command->get_id() <<
				" has been deleted by the session and is also taking part in deferred " <<
				nlohmann::json(result_command->get_type()).get<std::string>() << " command";

			throw std::runtime_error(error_msg.str());
		}
		static void throw_no_database()
		{
			throw std::invalid_argument("Cannot open a Session without specifying a name of a database "
				"to operate on. Database name can be passed as an argument when Session is"
				" being opened or default database can be defined using DocumentStore.database");
		}

	protected:
		explicit InMemoryDocumentSessionOperations(/*DocumentStoreBase documentStore, UUID id,*/ SessionOptions options)
			: id(0)//TODO set id
			, database_name([&]
		{
			if (impl::utils::is_blank(options.database))
				throw_no_database();
			return std::move(options.database);
			
		}())
			, _request_executor(options.request_executor)
			, no_tracking(options.no_tracking)
			, _transaction_mode(options.transaction_mode)
			, _sessionInfo(id, {}, options.no_caching)
		{}

		void store_entity_in_unit_of_work(std::optional<std::string> id, std::shared_ptr<void>entity,
			std::optional<std::string> change_vector, nlohmann::json metadata , 
			ConcurrencyCheckMode force_concurrency_check, DocumentInfo::ToJsonConverter to_json)
		{
			if (!to_json)
			{
				throw std::invalid_argument("to_json must have a target");
			}
			_deleted_entities.erase(entity);
			if(id)
			{
				_known_missing_ids.erase(*id);
			}

			auto doc_info = std::make_shared<DocumentInfo>();
			if (id)
			{
				doc_info->id = *id;
			}
			doc_info->metadata = std::move(metadata);
			if(change_vector)
			{
				doc_info->change_vector = *std::move(change_vector);
			}
			doc_info->concurrency_check_mode = force_concurrency_check;
			doc_info->entity = entity;
			doc_info->to_json_converter = to_json;
			doc_info->new_document = true;
			
			_documents_by_entity.insert({entity, doc_info});
			if(id)
			{
				_documents_by_id.insert({*id, doc_info});
			}
		}

	public:
		const auto& get_documents_by_id() const
		{
			return _documents_by_id;
		}
		const auto& get_included_documents_by_id() const
		{
			return _included_documents_by_id;
		}

		void increment_request_count()
		{
			if(++_number_of_requests > max_number_of_requests_per_session)
			{
				std::ostringstream error;
				error << "The maximum number of requests " << max_number_of_requests_per_session <<
					" allowed for this session has been reached." <<
					"Raven limits the number of remote calls that a session is allowed to make as an early warning system. Sessions are expected to be short lived, and "
					"Raven provides facilities like load(std::vector<std::string> keys) to load multiple documents at once and batch saves (call SaveChanges() only once)."
					"You can increase the limit by setting DocumentConvention.max_number_of_requests_per_session or 'max_number_of_requests_per_session', but it is"
					"advisable that you'll look into reducing the number of remote calls first, since that will speed up your application significantly and result in a"
					"more responsive application.";
				throw std::runtime_error(error.str());
			}
		}

		bool is_deleted(const std::string& id) const
		{
			return _known_missing_ids.find(id) != _known_missing_ids.end();
		}

		bool is_loaded_or_deleted(const std::string& id) const
		{
			auto doc_info = _documents_by_id.find(id);
			bool is_loaded = doc_info != _documents_by_id.end() && (!doc_info->second->document.empty() || doc_info->second->entity);

			return  is_loaded || is_deleted(id) || _included_documents_by_id.find(id) != _included_documents_by_id.end();
		}

		bool is_loaded(const std::string& id) const
		{
			return is_loaded_or_deleted(id);
		}

		template<typename T>
		std::shared_ptr<T> track_entity(const DocumentInfo& document_found)
		{
			return std::static_pointer_cast<T>(track_entity(
				document_found.id, document_found.document, document_found.metadata, no_tracking,
				document_found.from_json_converter ? document_found.from_json_converter : DocumentInfo::default_from_json<T>));	
		}

		std::shared_ptr<void> track_entity(const std::string& id, const nlohmann::json& document, 
			const nlohmann::json& metadata, bool no_tracking_, DocumentInfo::FromJsonConverter from_json_converter)
		{
			if(!from_json_converter)
			{
				throw std::invalid_argument("from_json_converter must have a target");
			}

			no_tracking_ = this->no_tracking || no_tracking_;// if noTracking is session-wide then we want to override the passed argument

			if(id.empty())
			{
				//TODO something
			}

			
			if(auto doc_info = _documents_by_id.find(id); 
				doc_info != _documents_by_id.end())
			{// the local instance may have been changed, we adhere to the current Unit of Work
             // instance, and return that, ignoring anything new.
				if(!doc_info->second->entity)
				{
					doc_info->second->entity = from_json_converter(document);
				}
				if(!no_tracking_)
				{
					_included_documents_by_id.erase(id);
					_documents_by_entity.insert({ doc_info->second->entity,doc_info->second });
				}
				return doc_info->second->entity;
			}

			//TODO if(auto doc_info = _included_documents_by_id.find(id);

			auto entity = from_json_converter(document);

			std::string change_vector{};
			if(!impl::utils::json_utils::get_val_from_json(metadata, constants::documents::metadata::CHANGE_VECTOR, change_vector))
			{
				throw std::invalid_argument("Document "+ id + "must have Change Vector");
			}

			auto doc_info = std::make_shared<DocumentInfo>();
			doc_info->id = id;
			doc_info->document = document;
			doc_info->metadata = metadata;
			doc_info->entity = entity;
			doc_info->change_vector = change_vector;

			_documents_by_id.insert({ doc_info->id, doc_info });
			_documents_by_entity.insert({ entity,doc_info });

			return entity;
		}

		template<typename T>
		void store(std::shared_ptr<T> entity, std::optional<std::string> id = {},
			std::optional<DocumentInfo::ToJsonConverter> to_json = {})
		{
			store_internal(std::static_pointer_cast<void>(entity), {}, std::move(id),
				ConcurrencyCheckMode::AUTO, to_json ? *to_json : DocumentInfo::default_to_json<T>);
		}

		template<typename T>
		void delete_document(std::shared_ptr<T> typed_entity)
		{
			auto entity = std::static_pointer_cast<void>(typed_entity);
			if(!entity)
			{
				throw std::invalid_argument("Entity cannot be null");
			}
			std::shared_ptr<DocumentInfo> doc_info{};
			if(auto doc_info_it = _documents_by_entity.find(entity);
				doc_info_it == _documents_by_entity.end())
			{
				throw std::runtime_error("This entity is not associated with the session, cannot delete unknown entity instance");
			}else
			{
				doc_info = doc_info_it->second;
			}

			_deleted_entities.insert(entity);
			_included_documents_by_id.erase(doc_info->id);

			_known_missing_ids.insert(doc_info->id);
		}

		template<typename T>
		void delete_document(const std::string& id, const std::optional<std::string>& expected_change_vector = {})
		{
			delete_document(id, expected_change_vector, DocumentInfo::default_to_json<T>);
		}
		
		void delete_document(const std::string& id, const std::optional<std::string>& expected_change_vector = {},
			std::optional<DocumentInfo::ToJsonConverter> to_json = {})
		{
			if(impl::utils::is_blank(id))
			{
				throw std::invalid_argument("Id cannot be blank");
			}

			std::string change_vector;
			if (auto doc_info_it = _documents_by_id.find(id);
				doc_info_it != _documents_by_id.end())
			{
				auto doc_info = doc_info_it->second;
				if(!to_json && !doc_info->to_json_converter)
				{
					throw std::invalid_argument("for document id = " + id + " to_json is not provided "
						"and missing in the session. "
						"Use template delete_document() for default to_json serializer.");
				}
				nlohmann::json new_obj = to_json ? (*to_json)(doc_info->entity) : doc_info->to_json_converter(doc_info->entity);
				//TODO if(auto empty_changes = std::unordered_map<std::string, std::vector<DocumentsChanges>>();
				//	doc_info->entity && entity_changed(new_obj), doc_info, empty_changes)
				//{
				//	throw std::runtime_error("Can't delete changed entity using identifier. "
				//		"Use template<typename T>void delete_document(std::shared_ptr<T> typed_entity) instead.");
				//}

				if(doc_info->entity)
				{
					_documents_by_entity.erase(doc_info->entity);
				}
				_documents_by_id.erase(id);
				change_vector = doc_info->change_vector;
			}

			_known_missing_ids.insert(id);
			if(!use_optimistic_concurrency)
			{
				change_vector = "";
			}

			defer({ std::make_shared<commands::batches::DeleteCommandData>
				(id, expected_change_vector ? *expected_change_vector : change_vector) });
		}

		SaveChangesData prepare_for_save_changes()
		{
			auto result = SaveChangesData{*this};

			_deferred_commands.clear();
			_deferred_commands_map.clear();

			auto empty_changes = std::optional<std::unordered_map<std::string, DocumentsChanges>>{};
			prepare_for_entities_deletion(result, empty_changes);
			prepare_for_entities_puts(result);

			//TODO prepare_compare_exchange_entities(result);

			if(!_deferred_commands.empty())
			{// this allow on_before_store to call defer during the call to include
            // additional values during the same save_changes call
				result.deferred_commands.insert(result.deferred_commands.end(), _deferred_commands.cbegin(), _deferred_commands.cend());
				result.deferred_commands_map.insert(_deferred_commands_map.cbegin(), _deferred_commands_map.cend());
			}

			_deferred_commands.clear();
			_deferred_commands_map.clear();

			//TODO
			//for(auto& deferred_command : result.deferred_commands)
			//{
			//	deferred_command->on_before_save_changes(*this);
			//}
			return std::move(result);
		}

		void defer(const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands)
		{
			_deferred_commands.insert(_deferred_commands.end(), commands.begin(), commands.end());
			for(auto command : commands)
			{
				defer_internal(command);
			}
		}




	};
	inline InMemoryDocumentSessionOperations::~InMemoryDocumentSessionOperations() = default;
}

