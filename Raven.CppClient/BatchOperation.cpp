#include "stdafx.h"
#include "BatchOperation.h"
#include "MetadataAsDictionary.h"
#include "DocumentInfo.h"
#include "json_utils.h"
#include "CommandType.h"
#include "BatchCommand.h"
#include "PatchStatus.h"
#include "RequestExecutor.h"
#include "AfterSaveChangesEventArgs.h"
#include "NotImplementedException.h"

namespace ravendb::client::documents::session::operations
{
	std::optional<commands::batches::BatchCommand> BatchOperation::create_request()
	{
		auto session = _session.lock();
		auto result = session->prepare_for_save_changes();

		_on_successful_request = result.on_success;
		_session_commands_count = result.session_commands.size();
		result.session_commands.insert(result.session_commands.end(),
			result.deferred_commands.cbegin(), result.deferred_commands.cend());

		session->validate_cluster_transaction(result);

		_all_commands_count = result.session_commands.size();
		if (_all_commands_count == 0)
		{
			return {};
		}

		session->increment_request_count();
		_entities = result.entities;

		return std::optional<commands::batches::BatchCommand>(std::in_place, session->get_conventions(),
			result.session_commands, result.options, session->get_transaction_mode());
	}

	void BatchOperation::set_result(const json::BatchCommandResult& result)
	{
		auto session = _session.lock();

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

		_on_successful_request->clear_session_state_after_successful_save_changes();

		if (session->get_transaction_mode() == TransactionMode::CLUSTER_WIDE &&
			result.transaction_index && result.transaction_index.value() <= 0)
		{
			throw exceptions::ClientVersionMismatchException("Cluster transaction was send to a node that is not supporting it. "
				"So it was executed ONLY on the requested node on " + 
				session->get_request_executor()->get_url().value_or(""));
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
				throw std::runtime_error("Command " + nlohmann::json(type).dump() +
					" is not supported");
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
				handle_attachment_put(batch_result);
				break;
			case commands::batches::CommandType::ATTACHMENT_DELETE:
				handle_attachment_delete(batch_result);
				break;
			case commands::batches::CommandType::ATTACHMENT_MOVE:
				handle_attachment_move(batch_result);
				break;
			case commands::batches::CommandType::ATTACHMENT_COPY:
				handle_attachment_copy(batch_result);
				break;
			case commands::batches::CommandType::COMPARE_EXCHANGE_PUT:
			case commands::batches::CommandType::COMPARE_EXCHANGE_DELETE:
				break;
			case commands::batches::CommandType::COUNTERS:
				//TODO handle_counters(batch_result);
				throw exceptions::NotImplementedException();
				break;
			case commands::batches::CommandType::BATCH_PATCH:
				break;
			default:
				throw std::runtime_error("Command " + nlohmann::json(type).dump() +
					" is not supported");
			}
		}
		finalize_result();
	}

	void BatchOperation::handle_attachment_put(const nlohmann::json& batch_result)
	{
		handle_attachment_put_internal(batch_result, commands::batches::CommandType::ATTACHMENT_PUT,
			"Id", "Name");
	}

	void BatchOperation::handle_attachment_put_internal(const nlohmann::json& batch_result,
		commands::batches::CommandType type, const std::string& id_field_name, const std::string& attachment_name_field_name)
	{
		auto&& id = get_string_field(batch_result, type, id_field_name);

		std::shared_ptr<DocumentInfo> session_document_info{};
		auto session = _session.lock();
		if(auto it = session->_documents_by_id.find(id);
			it != session->_documents_by_id.end())
		{
			session_document_info = it->second;
		}
		else
		{
			return;
		}

		auto document_info = get_or_add_modifications(id, session_document_info, false);
		auto& attachments = document_info->metadata[constants::documents::metadata::ATTACHMENTS];

		nlohmann::json dynamic_node = nlohmann::json::object();

		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(dynamic_node, "ChangeVector", get_string_field(batch_result, type, "ChangeVector"));
		set_val_to_json(dynamic_node, "ContentType", get_string_field(batch_result, type, "ContentType"));
		set_val_to_json(dynamic_node, "Hash", get_string_field(batch_result, type, "Hash"));
		set_val_to_json(dynamic_node, "Name", get_string_field(batch_result, type, "Name"));
		set_val_to_json(dynamic_node, "Size", get_long_field(batch_result, type, "Size"));

		attachments.push_back(std::move(dynamic_node));
	}

	void BatchOperation::handle_attachment_delete(const nlohmann::json& batch_result)
	{
		handle_attachment_delete_internal(batch_result, commands::batches::CommandType::ATTACHMENT_DELETE,
			constants::documents::metadata::ID, "Name");
	}

	void BatchOperation::handle_attachment_delete_internal(const nlohmann::json& batch_result,
		commands::batches::CommandType type, const std::string& id_field_name,
		const std::string& attachment_name_field_name)
	{
		auto&& id = get_string_field(batch_result, type, id_field_name);

		std::shared_ptr<DocumentInfo> session_document_info{};
		auto session = _session.lock();
		if (auto it = session->_documents_by_id.find(id);
			it != session->_documents_by_id.end())
		{
			session_document_info = it->second;
		}
		else
		{
			return;
		}

		auto document_info = get_or_add_modifications(id, session_document_info, false);

		if(auto it = document_info->metadata.find(constants::documents::metadata::ATTACHMENTS);
			it == document_info->metadata.end() || it->is_null() || it->empty() )
		{
			return;
		}

		auto& attachments_json = document_info->metadata.at(constants::documents::metadata::ATTACHMENTS);
		auto&& name = get_string_field(batch_result, type, attachment_name_field_name);

		nlohmann::json new_attachments_json = nlohmann::json::array();

		for(const auto& attachment : attachments_json)
		{
			if(name == get_string_field(attachment, type, "Name"))
			{
				continue;
			}
			new_attachments_json.push_back(attachment);
		}
		attachments_json = std::move(new_attachments_json);
	}

	void BatchOperation::handle_attachment_move(const nlohmann::json& batch_result)
	{
		handle_attachment_delete_internal(batch_result, commands::batches::CommandType::ATTACHMENT_MOVE,
			"Id", "Name");
		handle_attachment_put_internal(batch_result, commands::batches::CommandType::ATTACHMENT_MOVE,
			"DestinationId", "DestinationName");
	}

	void BatchOperation::handle_attachment_copy(const nlohmann::json& batch_result)
	{
		handle_attachment_put_internal(batch_result, commands::batches::CommandType::ATTACHMENT_COPY,
			"Id", "Name");
	}

	void BatchOperation::handle_put(size_t index, const nlohmann::json& batch_result, bool is_deferred)
	{
		auto session = _session.lock();
		std::shared_ptr<void> entity{};
		std::shared_ptr<DocumentInfo> doc_info{};
		if (!is_deferred)
		{
			entity = _entities.at(index);
			if (const auto doc_info_it = session->_documents_by_entity.find(entity);
				doc_info_it == session->_documents_by_entity.end())
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
			const auto session_document_info_it = session->_documents_by_id.find(id);
			if (session_document_info_it == session->_documents_by_id.end())
			{
				return;
			}

			doc_info = get_or_add_modifications(id, session_document_info_it->second, true);
			entity = doc_info->entity;
		}

		for (const auto&[key, val] : batch_result.items())
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
		session->_documents_by_id.insert_or_assign(doc_info->id, doc_info);

		if (entity && doc_info->stored_type)
		{
			session->get_generate_entity_id_on_the_client().try_set_identity(*doc_info->stored_type, entity, id);
		}

		auto&& after_save_changes_event_args = AfterSaveChangesEventArgs(_session.lock(), doc_info->id, doc_info->entity);
		_session.lock()->on_after_save_changes_invoke(after_save_changes_event_args);
	}

	void BatchOperation::handle_patch(const nlohmann::json& batch_result)
	{
		auto session = _session.lock();

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

			if (auto session_document_info_it = session->_documents_by_id.find(id);
				session_document_info_it == session->_documents_by_id.end())
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
				session->get_entity_to_json().populate_entity(document_info->entity, id, document_info->document,
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
		auto session = _session.lock();

		auto&& id = get_string_field(batch_result, type, "Id");
		const auto doc_info_it = session->_documents_by_id.find(id);
		if (doc_info_it == session->_documents_by_id.end())
		{
			return;
		}

		const auto entity = doc_info_it->second->entity;
		session->_documents_by_id.erase(id);
		if (entity)
		{
			session->_deleted_entities.erase(entity);
			session->_deleted_entities.erase(entity);
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
		doc_info->metadata_instance = std::make_shared<json::MetadataAsDictionary>(doc_info->metadata);
		doc_info->document.at(constants::documents::metadata::KEY) = doc_info->metadata;
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

		throw std::runtime_error("Disabling a warning : shouldn't get here.");
	}

	int64_t BatchOperation::get_long_field(const nlohmann::json& j, commands::batches::CommandType type,
		const std::string& field_name)
	{
		int64_t field{};
		if (impl::utils::json_utils::get_val_from_json(j, field_name, field))
		{
			return field;
		}
		throw_missing_field(type, field_name);

		throw std::runtime_error("Disabling a warning : shouldn't get here.");
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
