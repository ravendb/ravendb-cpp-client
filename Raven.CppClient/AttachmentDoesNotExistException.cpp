#include "stdafx.h"
#include "AttachmentDoesNotExistException.h"

namespace ravendb::client::exceptions::attachments
{
	AttachmentDoesNotExistException::AttachmentDoesNotExistException() = default;

	AttachmentDoesNotExistException::AttachmentDoesNotExistException(const std::string& message)
		: RavenException(message)
	{}

	void AttachmentDoesNotExistException::throw_for(const std::string& document_id, const std::string& attachment_name)
	{
		std::ostringstream msg{};
		msg << "There is no attachment with '" << attachment_name << "' name for document '" << document_id + "'";

		throw AttachmentDoesNotExistException(msg.str());
	}
}
