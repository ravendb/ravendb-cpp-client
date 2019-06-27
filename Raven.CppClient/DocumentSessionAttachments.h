#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "DocumentSessionAttachmentsBase.h"
#include "IAttachmentsSessionOperations.h"

namespace ravendb::client::documents::session
{
	class InMemoryDocumentSessionOperations;

	class DocumentSessionAttachments : public IAttachmentsSessionOperations, public DocumentSessionAttachmentsBase
	{
	public:
		~DocumentSessionAttachments() override = default;
		
		DocumentSessionAttachments(std::shared_ptr<InMemoryDocumentSessionOperations> session);

		std::vector<documents::operations::attachments::AttachmentName> get_attachments_names(std::shared_ptr<void> entity) const override;

		void store_attachment(const std::string& document_id, const std::string& name, std::istream& stream, const std::optional<std::string>& content_type) override;

		void store_attachment(std::shared_ptr<void> entity, const std::string& name, std::istream& stream, const std::optional<std::string>& content_type) override;

		void delete_attachment(const std::string& document_id, const std::string& name) override;

		void delete_attachment(std::shared_ptr<void> entity, const std::string& name) override;

		void rename_attachment(const std::string& document_id, const std::string& name, const std::string& new_name) override;

		void rename_attachment(std::shared_ptr<void> entity, const std::string& name, const std::string& new_name) override;

		void copy_attachment(const std::string& source_document_id, const std::string& source_name, const std::string& destination_document_id, const std::string& destination_name) override;

		void copy_attachment(std::shared_ptr<void> source_entity, const std::string& source_name, std::shared_ptr<void> destination_entity, const std::string& destination_name) override;

		void move_attachment(const std::string& source_document_id, const std::string& source_name, const std::string& destination_document_id, const std::string& destination_name) override;

		void move_attachment(std::shared_ptr<void> source_entity, const std::string& source_name, std::shared_ptr<void> destination_entity, const std::string& destination_name) override;

		bool exists(const std::string& document_id, const std::string& name) override;

		std::shared_ptr<documents::operations::attachments::AttachmentResult> get(const std::string& document_id, const std::string& name) override;

		std::shared_ptr<documents::operations::attachments::AttachmentResult> get(std::shared_ptr<void> entity, const std::string& name) override;
	};
}
