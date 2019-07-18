#include "stdafx.h"
#include "CompactSettings.h"
#include "json_utils.h"

namespace  ravendb::client::serverwide
{
	void to_json(nlohmann::json& j, const CompactSettings& cs)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		if (cs.node)
		{
			set_val_to_json(j, "ServerNode", *cs.node);
		}
		else
		{
			set_val_to_json(j, "ServerNode", nullptr);
		}
		set_val_to_json(j, "DatabaseName", cs.database_name);
		set_val_to_json(j, "Documents", cs.documents);
		set_val_to_json(j, "Indexes", cs.indexes);
	}
}
