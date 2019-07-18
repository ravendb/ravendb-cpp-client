#include "stdafx.h"
#include "PatchResult.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	void from_json(const nlohmann::json& j, PatchResult& pr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Status", pr.status);
		get_val_from_json(j, "ModifiedDocument", pr.modified_document);
		get_val_from_json(j, "OriginalDocument", pr.original_document);
		get_val_from_json(j, "Debug", pr.debug);
		get_val_from_json(j, "LastModified", pr.last_modified);
		get_val_from_json(j, "ChangeVector", pr.change_vector);
		get_val_from_json(j, "Collection", pr.collection);
	}
}
