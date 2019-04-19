#include "stdafx.h"
#include "BatchOperation.h"
#include "MetadataAsDictionary.h"
#include "DocumentInfo.h"
#include "json_utils.h"
#include "InMemoryDocumentSessionOperations.h"
#include "CommandType.h"
#include "BatchCommand.h"
#include "PatchStatus.h"
#include "RequestExecutor.h"

namespace ravendb::client::documents::session::operations
{
	std::optional<commands::batches::BatchCommand> BatchOperation::create_request()
	{
		auto result = _session.lock()->prepare_for_save_changes();
		_session_commands_count = result.session_commands.size();
		result.session_commands.insert(result.session_commands.end(),
			result.deferred_commands.cbegin(), result.deferred_commands.cend());

		//TODO _session.validate_cluster_transaction(result);
		_all_commands_count = result.session_commands.size();
		if (_all_commands_count == 0)
		{
			return {};
		}

		_session.lock()->increment_request_count();
		_entities = result.entities;

		return std::optional<commands::batches::BatchCommand>(std::in_place, _session.lock()->get_conventions(),
			result.session_commands, result.options, _session.lock()->get_transaction_mode());
	}

	void BatchOperation::set_result(const json::BatchCommandResult& result)
	{
		static const auto get_command_type = [](const nlohmann::json::object_t& batch_result)
		{
			auto type_it = batch_result.find("Type");
			if (type_it == batch_result.end() || !type_it->second.is_string())
			{
				return commands::batches::CommandType::NONE;
			}
			return type_it->second.get<commands::batches::CommandType>();
		};

		if (result.results.empty())
		{
			throw_on_empty_results();
		}

		if (_session.lock()->get_transaction_mode() == TransactionMode::CLUSTER_WIDE &&
			result.transaction_index && result.transaction_index.value() <= 0)
		{
			//TODO change to ClientVersionMismatchException
			throw std::runtime_error("Cluster transaction was send to a node that is not supporting it. "
				"So it was executed ONLY on the requested node on "/* + _session->get_request_executor()->get_url()*/);
		}

		if (result.results.size() < _session_commands_count)
		{
			throw std::runtime_error("Batch command results are less then _session_commands_count");
		}
		for (size_t i = 0; i < _session_commands_count; ++i)
		{
			const auto& batch_result = result.results[i];
			if (!batch_result.is_object())
			{
				continue;//OK ?
			}

			auto type = get_command_type(batch_result);

			switch (type)
			{
			case commands::batches::CommandType::PUT:
				handle_put(i, batch_result, false);
				break;
			case commands::batches::CommandType::DELETE_:
				handle_delete(batch_result);
				break;
			case commands::batches::CommandType::COMPARE_EXCHANGE_PUT:
			case commands::batches::CommandType::COMPARE_EXCHANGE_DELETE:
				break;
			default:
				throw std::runtime_error("Command " + nlohmann::json(type).get<std::string>() +
					"is not supported");
			}
		}

		if (auto&& res_size = result.results.size();
			res_size >= _session_commands_count && res_size < _all_commands_count)
		{
			throw std::runtime_error("Batch command results are less then _all_commands_count");
		}

		for (size_t i = _session_commands_count; i < _all_commands_count; ++i)
		{
			const auto& batch_result = result.results[i];
			if (!batch_result.is_object())
			{
				continue;//OK ?
			}

			auto type = get_command_type(batch_result);

			switch (type)
			{
			case commands::batches::CommandType::PUT:
				handle_put(i, batch_result, true);
				break;
			case commands::batches::CommandType::DELETE_:
				handle_delete(batch_result);
				break;
			case commands::batches::CommandType::PATCH:
				handle_patch(batch_result);
				break;
			case commands::batches::CommandType::ATTACHMENT_PUT:
				//TODO handle_attachment_put(batch_result);
				break;
			case commands::batches::CommandType::ATTACHMENT_DELETE:
				//TODO handle_attachment_delete(batch_result);
				throw std::runtime_error("not implemented");
				break;
			case commands::batches::CommandType::ATTACHMENT_MOVE:
				//TODO handle_attachment_move(batch_result);
				throw std::runtime_error("not implemented");
				break;
			case commands::batches::CommandType::ATTACHMENT_COPY:
				//TODO handle_attachment_copy(batch_result);
				throw std::runtime_error("not implemented");
				break;
			case commands::batches::CommandType::COMPARE_EXCHANGE_PUT:
			case commands::batches::CommandType::COMPARE_EXCHANGE_DELETE:
				break;
			case commands::batches::CommandType::COUNTERS:
				//TODO handle_counters(batch_result);
				throw std::runtime_error("not implemented");
				break;
			case commands::batches::CommandType::BATCH_PATCH:
				break;
			default:
				throw std::runtime_error("Command " + nlohmann::json(type).get<std::string>() +
					"is not supported");
			}
		}
		finalize_result();
	}

	void BatchOperation::handle_put(size_t index, const nlohmann::json& batch_result, bool is_deferred)
	{
		std::shared_ptr<void> entity{};
		std::shared_ptr<DocumentInfo> doc_info{};
		if (!is_deferred)
		{
			entity = _entities.at(index);
			if (const auto doc_info_it = _session.lock()->_documents_by_entity.find(entity);
				doc_info_it == _session.lock()->_documents_by_entity.end())
			{
				return;
			}
			else
			{
				doc_info = doc_info_it->second;
			}
		}

		auto&& id = get_string_field(batch_result, commands::batches::CommandType::PUT,
			constants::documents::metadata::ID);
		auto&& change_vector = get_string_field(batch_result, commands::batches::CommandType::PUT,
			constants::documents::metadata::CHANGE_VECTOR);

		if (is_deferred)
		{
			const auto session_document_info_it = _session.lock()->_documents_by_id.find(id);
			if (session_document_info_it == _session.lock()->_documents_by_id.end())
			{
				return;
			}

			doc_info = get_or_add_modifications(id, session_document_info_it->second, true);
			entity = doc_info->entity;
		}

		for (const auto&[key, val] : static_cast<const nlohmann::json::object_t&>(batch_result))
		{
			static const std::string type_str = "Type";
			if (type_str == key)
			{
				continue;
			}
			impl::utils::json_utils::set_val_to_json(doc_info->metadata, key, val);
		}

		doc_info->id = id;
		doc_info->change_vector = change_vector;

		apply_metadata_modifications(id, doc_info);
		_session.lock()->_documents_by_id.insert_or_assign(doc_info->id, doc_info);

		if (entity && doc_info->stored_type)
		{
			_session.lock()->get_generate_entity_id_on_the_client().try_set_identity(*doc_info->stored_type, entity, id);
		}

		//TODO
		//AfterSaveChangesEventArgs afterSaveChangesEventArgs = new AfterSaveChangesEventArgs(_session, documentInfo.getId(), documentInfo.getEntity());
		//_session.onAfterSaveChangesInvoke(afterSaveChangesEventArgs);
	}

	void BatchOperation::handle_patch(const nlohmann::json& batch_result)
	{
		documents::operations::PatchStatus status = documents::operations::PatchStatus::UNSET;
		if(auto patch_status_it = batch_result.find("PatchStatus");
			patch_status_it == batch_result.end() || patch_status_it->is_null())
		{
			throw_missing_field(commands::batches::CommandType::PATCH, "PatchStatus");
		}else
		{
			status = patch_status_it->get<documents::operations::PatchStatus>();
		}
		switch (status)
		{
		case documents::operations::PatchStatus::CREATED:
		case documents::operations::PatchStatus::PATCHED:
		{
			nlohmann::json document{};
			if (auto doc_it = batch_result.find("ModifiedDocument");
				doc_it == batch_result.end())
			{
				return;
			}
			else
			{
				document = *doc_it;
			}

			auto id = get_string_field(batch_result, commands::batches::CommandType::PUT, "Id");
			std::shared_ptr<DocumentInfo> document_info{};

			if (auto session_document_info_it = _session.lock()->_documents_by_id.find(id);
				session_document_info_it == _session.lock()->_documents_by_id.end())
			{
				return;
			}
			else
			{
				document_info = get_or_add_modifications(id, session_document_info_it->second, true);
			}
			const auto change_vector = get_string_field(batch_result, commands::batches::CommandType::PATCH, "ChangeVector");
			const auto last_modified = get_string_field(batch_result, commands::batches::CommandType::PATCH, "LastModified");

			using impl::utils::json_utils::set_val_to_json;

			document_info->change_vector = get_string_field(batch_result, commands::batches::CommandType::PATCH, "ChangeVector");
			set_val_to_json(document_info->metadata, constants::documents::metadata::ID, id);
			set_val_to_json(document_info->metadata, constants::documents::metadata::CHANGE_VECTOR, change_vector);
			set_val_to_json(document_info->metadata, constants::documents::metadata::LAST_MODIFIED, last_modified);

			document_info->document = std::move(document);
			apply_metadata_modifications(id, document_info);

			if (document_info->entity)
			{
				_session.lock()->get_entity_to_json().populate_entity(document_info->entity, id, document_info->document,
					document_info->update_from_json);
			}
		}
			break;
		default:
			break;
		}
	}


	void BatchOperation::handle_delete(const nlohmann::json& batch_result)
	{
		handle_delete_internal(batch_result, commands::batches::CommandType::DELETE_);
	}


	void BatchOperation::handle_delete_internal(const nlohmann::json& batch_result, commands::batches::CommandType type)
	{
		auto&& id = get_string_field(batch_result, type, "Id");
		const auto doc_info_it = _session.lock()->_documents_by_id.find(id);
		if (doc_info_it == _session.lock()->_documents_by_id.end())
		{
			return;
		}

		const auto entity = doc_info_it->second->entity;
		_session.lock()->_documents_by_id.erase(id);
		if (entity)
		{
			_session.lock()->_deleted_entities.erase(entity);
			_session.lock()->_deleted_entities.erase(entity);
		}
	}

	void BatchOperation::finalize_result()
	{
		if (_modifications.empty())
		{
			return;
		}
		for (const auto&[id, doc_info] : _modifications)
		{
			apply_metadata_modifications(id, doc_info);
		}
	}

	void BatchOperation::apply_metadata_modifications(const std::string& id, std::shared_ptr<DocumentInfo> doc_info)
	{
		doc_info->metadata_instance = std::static_pointer_cast<IMetadataDictionary>
			(json::MetadataAsDictionary::create(doc_info->metadata));
	}

	std::shared_ptr<DocumentInfo> BatchOperation::get_or_add_modifications(const std::string& id, std::shared_ptr<DocumentInfo> doc_info, bool apply_modifications)
	{
		auto modified_doc_info = std::shared_ptr<DocumentInfo>{};

		if (auto modified_doc_info_it = _modifications.find(id);
			modified_doc_info_it != _modifications.end())
		{
			modified_doc_info = modified_doc_info_it->second;
			if (apply_modifications)
			{
				apply_metadata_modifications(id, modified_doc_info);
			}
		}
		else
		{
			_modifications.insert_or_assign(id, modified_doc_info = doc_info);
		}

		return modified_doc_info;
	}

	std::string BatchOperation::get_string_field(const nlohmann::json& j, commands::batches::CommandType type, const std::string& field_name)
	{
		std::string field{};
		if (impl::utils::json_utils::get_val_from_json(j, field_name, field))
		{
			return field;
		}
		throw_missing_field(type, field_name);
		return {};//shouldn't get here
	}

	void BatchOperation::throw_missing_field(commands::batches::CommandType type, const std::string& field_name)
	{
		std::ostringstream message;
		message << nlohmann::json(type).get<std::string>() <<
			" response is invalid. Field '" << field_name << "' is missing.";

		throw std::runtime_error(message.str());
	}

	void BatchOperation::throw_on_empty_results()
	{
		throw std::runtime_error("Received empty response from the server."
			" This is not supposed to happen and is likely a bug.");
	}

}
