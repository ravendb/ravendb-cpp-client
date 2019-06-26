#include "stdafx.h"
#include "AttachmentName.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::attachments
{
	void to_json(nlohmann::json& j, const AttachmentName& an)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", an.name);
		set_val_to_json(j, "Hash", an.hash);
		set_val_to_json(j, "ContentType", an.content_type);
		set_val_to_json(j, "Size", an.size);
	}

	void from_json(const nlohmann::json& j, AttachmentName& an)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", an.name);
		get_val_from_json(j, "Hash", an.hash);
		get_val_from_json(j, "ContentType", an.content_type);
		get_val_from_json(j, "Size", an.size);
	}
}
