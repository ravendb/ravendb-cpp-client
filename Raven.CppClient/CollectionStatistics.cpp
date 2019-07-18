#include "stdafx.h"
#include "CollectionStatistics.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	void from_json(const nlohmann::json& j, CollectionStatistics& cs)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "CountOfDocuments", cs.count_of_documents);
		get_val_from_json(j, "CountOfConflicts", cs.count_of_conflicts);
		get_val_from_json(j, "Collections", cs.collections);
	}
}
