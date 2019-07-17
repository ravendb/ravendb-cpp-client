#include "stdafx.h"
#include "DocumentSessionAttachmentsBase.h"
#include "AttachmentResult.h"
#include "utils.h"
#include "InMemoryDocumentSessionOperations.h"
#include "CommandType.h"
#include "PutAttachmentCommandData.h"
#include "DeleteAttachmentCommandData.h"
#include "CompareStringsEqualIgnoreCase.h"
#include "MoveAttachmentCommandData.h"
#include "CopyAttachmentCommandData.h"

namespace ravendb::client::documents::session
{
	void DocumentSessionAttachmentsBase::throw_document_already_deleted(const std::string& document_id,
		const std::string& name, const std::string& operation, const std::string& destination_document_id,
		const std::string& deleted_document_id)
	{
		std::ostringstream msg{};
		msg << "Can't " << operation << " attachment '" << name << "' of document '" << document_id << "' ";
		if (!destination_document_id.empty())
		{
			msg << " to '" << destination_document_id << "'";
		}
		msg << ", the document '" << deleted_document_id + "' was already deleted in this session";

		throw std::runtime_error(msg.str());
	}

	void DocumentSessionAttachmentsBase::throw_other_deferred_command_exception(const std::string& document_id,
		const std::string& name, const std::string& operation, const std::string& previous_operation)
	{
		std::ostringstream msg{};
		msg << "Can't " << operation << " attachment '" << name << "' of document '"
			<< document_id << "', there is a deferred command registered to " 
			<< previous_operation + " an attachment with '" + name + "' name.";

		throw std::runtime_error(msg.str());
	}

	DocumentSessionAttachmentsBase::DocumentSessionAttachmentsBase(
		std::shared_ptr<InMemoryDocumentSessionOperations> session)
		: AdvancedSessionExtentionBase(session)
	{}

	std::vector<documents::operations::attachments::AttachmentName> DocumentSessionAttachmentsBase::get_attachments_names(
		std::shared_ptr<void> entity) const
	{
		if(!entity)
		{
			return {};
		}

		std::shared_ptr<DocumentInfo> document{};

		if(auto it = documents_by_entity.find(entity);
			it != documents_by_entity.end())
		{
			document = it->second;
		}
		else
		{
			throw_entity_not_in_session();
		}
		const auto metadata = static_cast<const nlohmann::json::object_t&>(document->metadata);
		if(auto it = metadata.find(constants::documents::metadata::ATTACHMENTS);
			it == metadata.end())
		{
			return {};
		}
		else
		{
			std::vector<documents::operations::attachments::AttachmentName> results{};
			results.reserve(it->second.size());

			for(const auto& attachment : it->second)
			{
				results.emplace_back(attachment.get<documents::operations::attachments::AttachmentName>());
			}
			return results;
		}
	}

	void DocumentSessionAttachmentsBase::store_attachment(const std::string& document_id, const std::string& name,
		std::istream& stream, std::optional<std::string> content_type)
	{
		if (impl::utils::is_blank(document_id))
		{
			throw std::invalid_argument("'document_id' cannot be empty");
		}
		if (impl::utils::is_blank(name))
		{
			throw std::invalid_argument("'name' cannot be empty");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			document_id, commands::batches::CommandType::DELETE_, "" }) != deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(document_id, name, "store", "delete");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			document_id, commands::batches::CommandType::ATTACHMENT_PUT, name }) != deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(document_id, name, "store", "create");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			document_id, commands::batches::CommandType::ATTACHMENT_DELETE, name }) != deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(document_id, name, "store", "delete");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			document_id, commands::batches::CommandType::ATTACHMENT_MOVE, name }) != deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(document_id, name, "store", "rename");
		}

		if (auto it = documents_by_id.find(document_id);
			it != documents_by_id.end() && deleted_entities.find(it->second->entity) != deleted_entities.end())
		{
			throw_document_already_deleted(document_id, name, "store", "", document_id);
		}

		defer({std::make_shared<commands::batches::PutAttachmentCommandData>(
			document_id, name, stream, std::move(content_type))});
	}

	void DocumentSessionAttachmentsBase::store_attachment(std::shared_ptr<void> entity, const std::string& name,
		std::istream& stream, std::optional<std::string> content_type)
	{
		if(auto it = documents_by_entity.find(entity);
			it == documents_by_entity.end())
		{
			throw_entity_not_in_session();
		}
		else
		{
			store_attachment(it->second->id, name, stream, std::move(content_type));
		}
	}

	void DocumentSessionAttachmentsBase::delete_attachment(const std::string& document_id, const std::string& name)
	{
		if (impl::utils::is_blank(document_id))
		{
			throw std::invalid_argument("'document_id' cannot be empty");
		}
		if (impl::utils::is_blank(name))
		{
			throw std::invalid_argument("'name' cannot be empty");
		}

		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			document_id, commands::batches::CommandType::DELETE_, "" }) != deferred_commands_map.end() ||
			deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			document_id, commands::batches::CommandType::ATTACHMENT_DELETE, name }) != deferred_commands_map.end())
		{
			return; //no-op
		}

		if (auto it = documents_by_id.find(document_id);
			it != documents_by_id.end() && deleted_entities.find(it->second->entity) != deleted_entities.end())
		{
			return; //no-op
		}

		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			document_id, commands::batches::CommandType::ATTACHMENT_PUT, name }) != deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(document_id, name, "delete", "create");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			document_id, commands::batches::CommandType::ATTACHMENT_MOVE, name }) != deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(document_id, name, "delete", "rename");
		}

		defer({ std::make_shared<commands::batches::DeleteAttachmentCommandData>(
			document_id, name) });
	}

	void DocumentSessionAttachmentsBase::delete_attachment(std::shared_ptr<void> entity, const std::string& name)
	{
		if (auto it = documents_by_entity.find(entity);
			it == documents_by_entity.end())
		{
			throw_entity_not_in_session();
		}
		else
		{
			delete_attachment(it->second->id, name);
		}
	}

	void DocumentSessionAttachmentsBase::rename_attachment(const std::string& document_id, const std::string& name,
		const std::string& new_name)
	{
		move_attachment(document_id, name, document_id, new_name);
	}

	void DocumentSessionAttachmentsBase::rename_attachment(std::shared_ptr<void> entity, const std::string& name,
		const std::string& new_name)
	{
		move_attachment(entity, name, entity, new_name);
	}

	void DocumentSessionAttachmentsBase::move_attachment(const std::string& source_document_id,
		const std::string& source_name, const std::string& destination_document_id, const std::string& destination_name)
	{
		if (impl::utils::is_blank(source_document_id))
		{
			throw std::invalid_argument("'source_document_id' cannot be empty");
		}
		if (impl::utils::is_blank(source_name))
		{
			throw std::invalid_argument("'source_name' cannot be empty");
		}
		if (impl::utils::is_blank(destination_document_id))
		{
			throw std::invalid_argument("'destination_document_id' cannot be empty");
		}
		if (impl::utils::is_blank(destination_name))
		{
			throw std::invalid_argument("'destination_name' cannot be empty");
		}

		if(impl::utils::CompareStringsEqualIgnoreCase()(source_document_id, destination_document_id) &&
			source_name == destination_name)
		{
			return; //no-op
		}

		if(auto it = documents_by_id.find(source_document_id);
			it != documents_by_id.end() && deleted_entities.find(it->second->entity) != deleted_entities.end())
		{
			throw_document_already_deleted(source_document_id, source_name, "move",
				destination_document_id, source_document_id);
		}
		if (auto it = documents_by_id.find(destination_document_id);
			it != documents_by_id.end() && deleted_entities.find(it->second->entity) != deleted_entities.end())
		{
			throw_document_already_deleted(source_document_id, source_name, "move",
				destination_document_id, destination_document_id);
		}

		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			source_document_id, commands::batches::CommandType::ATTACHMENT_DELETE, source_name })
				!= deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(source_document_id, source_name, "rename", "delete");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			source_document_id, commands::batches::CommandType::ATTACHMENT_MOVE, source_name })
				!= deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(source_document_id, source_name, "rename", "rename");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			destination_document_id, commands::batches::CommandType::ATTACHMENT_DELETE, destination_name })
				!= deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(destination_document_id, destination_name, "rename", "delete");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			destination_document_id, commands::batches::CommandType::ATTACHMENT_MOVE, destination_name })
				!= deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(destination_document_id, destination_name, "rename", "rename");
		}

		defer({ std::make_shared<commands::batches::MoveAttachmentCommandData>(
			source_document_id, source_name, destination_document_id, destination_name) });
	}

	void DocumentSessionAttachmentsBase::move_attachment(std::shared_ptr<void> source_entity,
		const std::string& source_name, std::shared_ptr<void> destination_entity, const std::string& destination_name)
	{
		if (!source_entity)
		{
			throw std::invalid_argument("'source_entity' cannot be empty");
		}
		if (!destination_entity)
		{
			throw std::invalid_argument("'destination_entity' cannot be empty");
		}

		std::shared_ptr<DocumentInfo> source_document{};
		std::shared_ptr<DocumentInfo> destination_document{};

		if(auto it = documents_by_entity.find(source_entity);
			it != documents_by_entity.end())
		{
			source_document = it->second;
		}
		else
		{
			throw_entity_not_in_session();
		}

		if(auto it = documents_by_entity.find(destination_entity);
			it != documents_by_entity.end())
		{
			destination_document = it->second;
		}
		else
		{
			throw_entity_not_in_session();
		}

		move_attachment(source_document->id, source_name, destination_document->id, destination_name);
	}

	void DocumentSessionAttachmentsBase::copy_attachment(const std::string& source_document_id,
		const std::string& source_name, const std::string& destination_document_id, const std::string& destination_name)
	{
		if (impl::utils::is_blank(source_document_id))
		{
			throw std::invalid_argument("'source_document_id' cannot be empty");
		}
		if (impl::utils::is_blank(source_name))
		{
			throw std::invalid_argument("'source_name' cannot be empty");
		}
		if (impl::utils::is_blank(destination_document_id))
		{
			throw std::invalid_argument("'destination_document_id' cannot be empty");
		}
		if (impl::utils::is_blank(destination_name))
		{
			throw std::invalid_argument("'destination_name' cannot be empty");
		}

		if (impl::utils::CompareStringsEqualIgnoreCase()(source_document_id, destination_document_id) &&
			source_name == destination_name)
		{
			return; //no-op
		}

		if (auto it = documents_by_id.find(source_document_id);
			it != documents_by_id.end() && deleted_entities.find(it->second->entity) != deleted_entities.end())
		{
			throw_document_already_deleted(source_document_id, source_name, "copy",
				destination_document_id, source_document_id);
		}
		if (auto it = documents_by_id.find(destination_document_id);
			it != documents_by_id.end() && deleted_entities.find(it->second->entity) != deleted_entities.end())
		{
			throw_document_already_deleted(source_document_id, source_name, "copy",
				destination_document_id, destination_document_id);
		}

		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			source_document_id, commands::batches::CommandType::ATTACHMENT_DELETE, source_name })
			!= deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(source_document_id, source_name, "copy", "delete");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			source_document_id, commands::batches::CommandType::ATTACHMENT_MOVE, source_name })
			!= deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(source_document_id, source_name, "copy", "rename");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			destination_document_id, commands::batches::CommandType::ATTACHMENT_DELETE, destination_name })
			!= deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(destination_document_id, destination_name, "copy", "delete");
		}
		if (deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName{
			destination_document_id, commands::batches::CommandType::ATTACHMENT_MOVE, destination_name })
			!= deferred_commands_map.end())
		{
			throw_other_deferred_command_exception(destination_document_id, destination_name, "copy", "rename");
		}

		defer({ std::make_shared<commands::batches::CopyAttachmentCommandData>(
			source_document_id, source_name, destination_document_id, destination_name) });
	}

	void DocumentSessionAttachmentsBase::copy_attachment(std::shared_ptr<void> source_entity,
		const std::string& source_name, std::shared_ptr<void> destination_entity, const std::string& destination_name)
	{
		if (!source_entity)
		{
			throw std::invalid_argument("'source_entity' cannot be empty");
		}
		if (!destination_entity)
		{
			throw std::invalid_argument("'destination_entity' cannot be empty");
		}

		std::shared_ptr<DocumentInfo> source_document{};
		std::shared_ptr<DocumentInfo> destination_document{};

		if (auto it = documents_by_entity.find(source_entity);
			it != documents_by_entity.end())
		{
			source_document = it->second;
		}
		else
		{
			throw_entity_not_in_session();
		}

		if (auto it = documents_by_entity.find(destination_entity);
			it != documents_by_entity.end())
		{
			destination_document = it->second;
		}
		else
		{
			throw_entity_not_in_session();
		}

		copy_attachment(source_document->id, source_name, destination_document->id, destination_name);
	}

	void DocumentSessionAttachmentsBase::throw_entity_not_in_session()
	{
		throw std::invalid_argument("The provided entity is not associated with the session. "
			"Use 'document_id' instead or track the entity in the session.");
	}
}
