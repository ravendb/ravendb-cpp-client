#pragma once
#include "AttachmentName.h"
#include "nlohmann/json.hpp"


namespace ravendb::client::documents::operations::attachments
{
	struct AttachmentDetails : public AttachmentName
	{
		std::optional<std::string> change_vector{};
		std::string document_id{};
	};

	void to_json(nlohmann::json& j, const AttachmentDetails& ad);

	void from_json(const nlohmann::json& j, AttachmentDetails& ad);
}
