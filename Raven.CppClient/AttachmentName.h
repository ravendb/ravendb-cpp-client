#pragma once

namespace ravendb::client::documents::operations::attachments
{
	struct AttachmentName
	{
		std::string name{};
		std::string hash{};
		std::string content_type{};
		int64_t size{};
	};

	void to_json(nlohmann::json& j, const AttachmentName& an);

	void from_json(const nlohmann::json& j, AttachmentName& an);
}
