#pragma once
#include "IAttachmentsSessionOperationsBase.h"

namespace ravendb::client::documents::operations::attachments
{
	class AttachmentResult;
}

namespace ravendb::client::documents::session
{
	//Attachments advanced synchronous session operations
	class IAttachmentsSessionOperations : public IAttachmentsSessionOperationsBase
	{
	public:
		virtual ~IAttachmentsSessionOperations() = 0;

		virtual bool exists(const std::string& document_id, const std::string& name) = 0;

		virtual std::shared_ptr<documents::operations::attachments::AttachmentResult> get(const std::string& document_id, const std::string& name) = 0;

		virtual std::shared_ptr<documents::operations::attachments::AttachmentResult> get(std::shared_ptr<void> entity, const std::string& name) = 0;

		//TODO
		//virtual operations::attachments::AttachmentResult get_revision(const std::string& document_id, const std::string& name) = 0;
	};

	inline IAttachmentsSessionOperations::~IAttachmentsSessionOperations() = default;
}
