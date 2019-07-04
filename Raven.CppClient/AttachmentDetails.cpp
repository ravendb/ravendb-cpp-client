#include "stdafx.h"
#include "AttachmentDetails.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::attachments
{
	void to_json(nlohmann::json& j, const AttachmentDetails& ad)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		to_json(j, static_cast<const AttachmentName&>(ad));

		set_val_to_json(j, "ChangeVector", ad.change_vector);
		set_val_to_json(j, "DocumentId", ad.document_id);
	}

	void from_json(const nlohmann::json& j, AttachmentDetails& ad)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		from_json(j, static_cast<AttachmentName&>(ad));

		get_val_from_json(j, "ChangeVector", ad.change_vector);
		get_val_from_json(j, "DocumentId", ad.document_id);
	}
}
