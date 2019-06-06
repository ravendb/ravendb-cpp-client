#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::attachments
{
	class AttachmentDoesNotExistException : public RavenException
	{
	public:
		~AttachmentDoesNotExistException() override = default;

		AttachmentDoesNotExistException();

		explicit AttachmentDoesNotExistException(const std::string& message);

		static void throw_for(const std::string& document_id, const std::string& attachment_name);
	};
}
