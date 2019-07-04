#pragma once
#include "AdvancedSessionExtentionBase.h"
#include "AttachmentName.h"

namespace ravendb::client::documents::session
{
	class DocumentSessionAttachmentsBase : public AdvancedSessionExtensionBase
	{
	private:
		static void throw_document_already_deleted(const std::string& document_id, const std::string& name,
			const std::string& operation, const std::string& destination_document_id, const std::string& deleted_document_id);

		static void throw_other_deferred_command_exception(const std::string& document_id, const std::string& name,
			const std::string& operation, const std::string& previous_operation);

	protected:
		DocumentSessionAttachmentsBase(std::shared_ptr<InMemoryDocumentSessionOperations> session);

		static void throw_entity_not_in_session();

	public:
		virtual ~DocumentSessionAttachmentsBase() = 0;

		std::vector<documents::operations::attachments::AttachmentName> get_attachments_names(std::shared_ptr<void> entity) const;

		void store_attachment(const std::string& document_id, const std::string& name, std::istream& stream,
			std::optional<std::string> content_type = {});

		void store_attachment(std::shared_ptr<void> entity, const std::string& name, std::istream& stream,
			std::optional<std::string> content_type = {});

		void delete_attachment(const std::string& document_id, const std::string& name);

		void delete_attachment(std::shared_ptr<void> entity, const std::string& name);

		void rename_attachment(const std::string& document_id, const std::string& name, const std::string& new_name);

		void rename_attachment(std::shared_ptr<void> entity, const std::string& name, const std::string& new_name);

		void move_attachment(const std::string& source_document_id, const std::string& source_name,
			const std::string& destination_document_id, const std::string& destination_name);

		void move_attachment(std::shared_ptr<void> source_entity, const std::string& source_name,
			std::shared_ptr<void> destination_entity, const std::string& destination_name);

		void copy_attachment(const std::string& source_document_id, const std::string& source_name,
			const std::string& destination_document_id, const std::string& destination_name);

		void copy_attachment(std::shared_ptr<void> source_entity, const std::string& source_name,
			std::shared_ptr<void> destination_entity, const std::string& destination_name);
	};

	inline DocumentSessionAttachmentsBase::~DocumentSessionAttachmentsBase() = default;
}
