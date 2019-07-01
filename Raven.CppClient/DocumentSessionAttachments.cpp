#include "stdafx.h"
#include "DocumentSessionAttachments.h"
#include "HeadAttachmentCommand.h"
#include "GetAttachmentOperation.h"

namespace ravendb::client::documents::session
{
	DocumentSessionAttachments::DocumentSessionAttachments(std::shared_ptr<InMemoryDocumentSessionOperations> session)
		: DocumentSessionAttachmentsBase(session)
	{}

	std::vector<documents::operations::attachments::AttachmentName> DocumentSessionAttachments::get_attachments_names(
		std::shared_ptr<void> entity) const
	{
		return DocumentSessionAttachmentsBase::get_attachments_names(entity);
	}

	void DocumentSessionAttachments::store_attachment(const std::string& document_id, const std::string& name,
		std::istream& stream, const std::optional<std::string>& content_type)
	{
		DocumentSessionAttachmentsBase::store_attachment(document_id, name, stream, content_type);
	}

	void DocumentSessionAttachments::store_attachment(std::shared_ptr<void> entity, const std::string& name,
		std::istream& stream, const std::optional<std::string>& content_type)
	{
		DocumentSessionAttachmentsBase::store_attachment(entity, name, stream, content_type);
	}

	void DocumentSessionAttachments::delete_attachment(const std::string& document_id, const std::string& name)
	{
		DocumentSessionAttachmentsBase::delete_attachment(document_id, name);
	}

	void DocumentSessionAttachments::delete_attachment(std::shared_ptr<void> entity, const std::string& name)
	{
		DocumentSessionAttachmentsBase::delete_attachment(entity, name);
	}

	void DocumentSessionAttachments::rename_attachment(const std::string& document_id, const std::string& name,
		const std::string& new_name)
	{
		DocumentSessionAttachmentsBase::rename_attachment(document_id, name, new_name);
	}

	void DocumentSessionAttachments::rename_attachment(std::shared_ptr<void> entity, const std::string& name,
		const std::string& new_name)
	{
		DocumentSessionAttachmentsBase::rename_attachment(entity, name, new_name);
	}

	void DocumentSessionAttachments::copy_attachment(const std::string& source_document_id,
		const std::string& source_name, const std::string& destination_document_id, const std::string& destination_name)
	{
		DocumentSessionAttachmentsBase::copy_attachment(source_document_id, source_name,
			destination_document_id, destination_name);
	}

	void DocumentSessionAttachments::copy_attachment(std::shared_ptr<void> source_entity,
		const std::string& source_name, std::shared_ptr<void> destination_entity, const std::string& destination_name)
	{
		DocumentSessionAttachmentsBase::copy_attachment(source_entity, source_name,
			destination_entity, destination_name);
	}

	void DocumentSessionAttachments::move_attachment(const std::string& source_document_id,
		const std::string& source_name, const std::string& destination_document_id, const std::string& destination_name)
	{
		DocumentSessionAttachmentsBase::move_attachment(source_document_id, source_name,
			destination_document_id, destination_name);
	}

	void DocumentSessionAttachments::move_attachment(std::shared_ptr<void> source_entity,
		const std::string& source_name, std::shared_ptr<void> destination_entity, const std::string& destination_name)
	{
		DocumentSessionAttachmentsBase::move_attachment(source_entity, source_name,
			destination_entity, destination_name);
	}

	bool DocumentSessionAttachments::exists(const std::string& document_id, const std::string& name)
	{
		auto command = commands::HeadAttachmentCommand(document_id, name);
		request_executor->execute(command, session_info);
		return command.get_result().operator bool();
	}

	std::shared_ptr<documents::operations::attachments::AttachmentResult> DocumentSessionAttachments::get_attachment(const std::string& document_id,
		const std::string& name)
	{
		auto operation = documents::operations::attachments::GetAttachmentOperation(
			document_id, name, attachments::AttachmentType::DOCUMENT);
		return session->get_operations()->send(operation, session_info);
	}

	std::shared_ptr<documents::operations::attachments::AttachmentResult> DocumentSessionAttachments::get_attachment(std::shared_ptr<void> entity,
		const std::string& name)
	{
		if(auto it = documents_by_entity.find(entity);
			it == documents_by_entity.end())
		{
			throw_entity_not_in_session();
			//just to disable a warning - shouldn't get here...
			throw std::runtime_error("Shouldn't get her -> a bug.");
		}
		else
		{
			auto operation = documents::operations::attachments::GetAttachmentOperation(
				it->second->id, name, attachments::AttachmentType::DOCUMENT);
			return session->get_operations()->send(operation, session_info);
		}
	}
}
