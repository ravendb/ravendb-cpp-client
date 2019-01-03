#pragma once
#include "RequestExecutor.h"
#include "OperationExecutor.h"
#include "TransactionMode.h"
#include "SessionInfo.h"
#include <unordered_set>
#include <any>
#include "DocumentInfo.h"
#include "DocumentsById.h"
#include "SessionOptions.h"
#include "Constants.h"

namespace ravendb::client::documents::session::operations {
	class LoadOperation;
}

namespace ravendb::client::documents::session
{
	class InMemoryDocumentSessionOperations
	{
	public:
		friend operations::LoadOperation;

		virtual ~InMemoryDocumentSessionOperations() = 0;

		//BatchOptions _save_changes_options;//TODO use this
		const int32_t id;
		const std::string database_name;
		int32_t max_number_of_requests_per_session{};
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
		std::unordered_map<std::string, std::shared_ptr<std::any>> _external_state{};

		//Translate between an ID and its associated entity
		DocumentsById _documents_by_id{};
		//Translate between an ID and its associated entity
		DocumentsById _included_documents_by_id{};
		//hold the data required to manage the data for RavenDB's Unit of Work
		std::unordered_map<std::shared_ptr<void>, std::shared_ptr<DocumentInfo>> _documents_by_entity{};

		std::unordered_set<std::shared_ptr<void>> deleted_entities{};

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

	private:
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
			bool is_loaded = doc_info != _documents_by_id.end() && (!doc_info->second->document.empty() || !doc_info->second->entity);

			return  is_loaded || is_deleted(id) || _included_documents_by_id.find(id) != _included_documents_by_id.end();
		}

		bool is_loaded(const std::string& id) const
		{
			return is_loaded_or_deleted(id);
		}

		template<typename T>
		std::shared_ptr<T> track_entity(const DocumentInfo& document_found)
		{
			static const DocumentInfo::FromJsonConverter default_from_json = [](const nlohmann::json& json)->std::shared_ptr<void>
			{
				auto temp = std::make_shared<T>();
				*temp = json;
				return std::static_pointer_cast<void>(temp);
			};

			return std::static_pointer_cast<T>(track_entity(document_found.id, document_found.document, document_found.metadata,
				no_tracking, document_found.from_json_converter ? document_found.from_json_converter : default_from_json));	
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





		




	};
	inline InMemoryDocumentSessionOperations::~InMemoryDocumentSessionOperations() = default;
}
