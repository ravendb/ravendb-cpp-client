#pragma once
#include "AttachmentName.h"

namespace ravendb::client::documents::session
{
	class IAttachmentsSessionOperationsBase
	{
	public:
		virtual ~IAttachmentsSessionOperationsBase() = 0;

		virtual std::vector<documents::operations::attachments::AttachmentName> get_attachments_names(std::shared_ptr<void> entity) const = 0;

		virtual void store_attachment(const std::string& document_id, const std::string& name, std::istream& stream,
			const std::optional<std::string>& content_type = {}) = 0;

		virtual void store_attachment(std::shared_ptr<void> entity, const std::string& name, std::istream& stream,
			const std::optional<std::string>& content_type = {}) = 0;

		virtual void delete_attachment(const std::string& document_id, const std::string& name) = 0;

		virtual void delete_attachment(std::shared_ptr<void> entity, const std::string& name) = 0;

		virtual void rename_attachment(const std::string& document_id, const std::string& name, const std::string& new_name) = 0;

		virtual void rename_attachment(std::shared_ptr<void> entity, const std::string& name, const std::string& new_name) = 0;

		virtual void copy_attachment(const std::string& source_document_id, const std::string& source_name,
			const std::string& destination_document_id, const std::string& destination_name) = 0;

		virtual void copy_attachment(std::shared_ptr<void> source_entity, const std::string& source_name,
			std::shared_ptr<void> destination_entity, const std::string& destination_name) = 0;

		virtual void move_attachment(const std::string& source_document_id, const std::string& source_name,
			const std::string& destination_document_id, const std::string& destination_name) = 0;

		virtual void move_attachment(std::shared_ptr<void> source_entity, const std::string& source_name,
			std::shared_ptr<void> destination_entity, const std::string& destination_name) = 0;
	};

	inline IAttachmentsSessionOperationsBase::~IAttachmentsSessionOperationsBase() = default;
}
