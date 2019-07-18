#include "stdafx.h"
#include "GetDocumentsResult.h"
#include "json_utils.h"

namespace ravendb::client::documents::commands
{
	void from_json(const nlohmann::json& j, GetDocumentsResult& gdr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Includes", gdr.includes);
		get_val_from_json(j, "Results", gdr.results);
		get_val_from_json(j, "NextPageStart", gdr.next_page_start);
	}
}
