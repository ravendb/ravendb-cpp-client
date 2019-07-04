#pragma once

namespace ravendb::client::documents::attachments
{
	enum class AttachmentType
	{
		DOCUMENT,
		REVISION,

		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(AttachmentType,
		{
			{AttachmentType::UNSET, nullptr},
			{AttachmentType::DOCUMENT, "Document"},
			{AttachmentType::REVISION, "Revision"}
		})
}
